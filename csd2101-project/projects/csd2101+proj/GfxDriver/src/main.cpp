/*!
@file    main.cpp
@author  Prasanna Ghali       (pghali@digipen.edu)
         Parminder Singh   (parminder.singh@digipen.edu)

All content (c) 2002 DigiPen Institute of Technology, all rights reserved.
*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */

//@todo Uncomment each of the following three lines as you progress through
// the problems [rasterization, first-person camera, view-frustum culling, frustum
// clipping] assigned for the final project. You can complete these tasks in any
// order. My implementations of 3D picking and shadows [not assigned but that
// work in the sample] are hidden from you. However, they rely on auxiliary
// functions gfxModel::ComputeModelBVSphere() and gfxSphere::Transform() - both
// these functions are defined in file submit.cpp.
// You must implement these functions first for all tasks except the
// first-person camera and rasterizer to work correctly!!!
#define YOUR_RASTERIZER
//#define YOUR_CAMERA
#define YOUR_CULLER_CLIPPER

#include  "../../GfxLib/gfx/GFX.h"
#include "submit.h"

// PBO implementation
#include <glhelper.h>
#include <blitquad.h>

#include <cstdlib>
#include <sstream>
#include <string>
#include <iostream>
#include <chrono>
#include <thread> // For std::this_thread::sleep_for

/*                                                                  constants
----------------------------------------------------------------------------- */

// default window parameters
int constexpr			sWindowWidth = 1280;
int constexpr			sWindowHeight = 780;
size_t constexpr	sWindowX = 0;
size_t constexpr	sWindowY = 0;

// default viewport parameters
float constexpr		sViewportWidth = static_cast<float>(sWindowWidth);
float constexpr	  sViewportHeight = static_cast<float>(sWindowHeight);
float const				sViewportX = 0.f;
float const				sViewportY = 0.f;

// default camera parameters
gfxVector3 const	sCameraPosition(0.f, 5.f, 20.f);
gfxVector3 const	sCameraTarget(0.f, 5.f, -1.f);
gfxVector3 const	sCameraUp(0.f, 1.f, 0.f);

gfxPlane const		sReceiverPlane(0.f, 1.f, 0.f, 0.f);

// default perspective projection parameters
float const				sFOVY = 45.f; // in degrees
float const				sAspectRatio = sViewportWidth / sViewportHeight;
float const				sNearDist = 1.f;
float const				sFarDist = 1000.f;

// camera displacements
float const				sCamStep = 0.5f;
float const				sCamOrientationDamper = 1.f / 100.f;

// frame rate
float const				sMaxFps = 60.f;	// throttled game fps

/*                                                    objects with file scope
----------------------------------------------------------------------------- */

//gfxCamera* sPCam = 0; // pointer to either built-in or YourCamera

// your implementations
#ifdef YOUR_RASTERIZER
YourRasterizer* gYourRasterizer = 0;
#endif

#ifdef YOUR_CAMERA
gfxCamera* sPCam = new YourCamera();
#else
gfxCamera* sPCam = new gfxCamera();
#endif

#ifdef YOUR_CULLER_CLIPPER
YourClipper* sYourClipper = 0;
#endif

// mouse, now driven by GLFW cursor-position callbacks
double            sMX = 0.0;
double            sMY = 0.0;

// toggles
bool              sMoveF = false;
bool              sMoveB = false;
bool              sMouseLook = false;
bool              sWireframe = false;
gfxInterpMode     sInterpMode = GFX_LINEAR_INTERP; //GFX_HYPERBOLIC_INTERP;

// steps
float							sAngle = PI;
float const				sMeshSpinStep = 0.01f;
int								sPickID = -1;
int								sReceiverIdx;
int								sLightIdx;

// device
gfxGraphicsPipe* sSWPipe = 0; // software graphics pipe
gfxObjectList     sScene;      // object list

// presentation (GL display of the CPU-rasterized framebuffer)
BlitQuad          sBlitQuad;

// models
gfxModel* sCubeModelP = 0;;
gfxModel* sSphereHRMP = 0;
gfxModel* sSphereLRMP = 0;
gfxModel* sGndModelP = 0;

/*                                                  functions with file scope
----------------------------------------------------------------------------- */
static void InitScene();
static void DrawScene(std::stringstream&);
static void UpdateScene();
static void HandleKey(int key, int action);
static void HandleMouseButton(int button, int action, double x, double y);
static void HandleMousePos(double x, double y);

/*                                                                  functions
----------------------------------------------------------------------------- */

/*  _________________________________________________________________________ */
void UpdateScene()
/*!
        Read user input and updates camera orientation and position
*/
{
    double cx = sMX, cy = sMY;
    if (GLHelper::ptr_window) {
        glfwGetCursorPos(GLHelper::ptr_window, &cx, &cy);
    }

    if (sMouseLook) {
        // Using changes in mouse position between previous and current frame,
        // measure in radians, changes in latitudinal and azimuth angles.
        float  deltaAlpha = static_cast<float>(cx - sMX) * sCamOrientationDamper;
        float  deltaBeta = static_cast<float>(cy - sMY) * sCamOrientationDamper;

        // Orient.
        sPCam->SetAzimuth(sPCam->GetAzimuth() - deltaAlpha);
        sPCam->SetLatitude(sPCam->GetLatitude() - deltaBeta);

        // Move.
        if (sMoveF) {
            sPCam->Move(0.f, 0.f, sCamStep);
        }
        if (sMoveB) {
            sPCam->Move(0.f, 0.f, -sCamStep);
        }
    }
    else if (sPickID >= 0 && sPickID < static_cast<int>(sScene.size())) {
        gfxVector3 basis(1.f, 0.f, 0.f);
        gfxVector3  basis_x(1.f, 0.f, 0.f);
        gfxVector3  basis_y(0.f, 1.f, 0.f);
        gfxVector3  basis_z(0.f, 0.f, 1.f);
        gfxVector3  pos = sScene[sPickID].GetWorldPosition();

        // Get basis vectors for the current camera.
        sPCam->GetBasisVectors(&basis_x, &basis_y, &basis_z);

        // This allows us to move the object in world space intuitively.
        // Remember to invert basis Y because it increases upwards and
        // screen coordinates increase downward.
        pos += (static_cast<float>(cx - sMX) / 50.f) * basis_x;
        pos += (static_cast<float>(cy - sMY) / 50.f) * -basis_y;
        sScene[sPickID].SetWorldPosition(pos.x, ((pos.y <= 1.75f) ? 2.f : pos.y), pos.z);
    }

    for (size_t i = 0; i < sScene.size(); ++i) {
        if (i == sLightIdx) {
            sAngle += 0.05f;
            gfxVector3 pos(static_cast<float>(5.0 * sin(sAngle)),
                sScene[sLightIdx].GetWorldPosition().y,
                static_cast<float>(5.0 * cos(sAngle)) - 5.f);
            sScene[i].SetWorldPosition(pos);
        }

        sScene[i].Update();
    }

    // Update cursor position.
    sMX = cx;
    sMY = cy;
}

/*  _________________________________________________________________________ */
void DrawScene(std::stringstream& fmt)
/*!
        Sets render states: depth buffer, lighting, shadows, ...
        Passes vertex and triangle lists to transform engine and
        subsequently to pixel engine

    @param fmt -->  Any text to write to viewport.
*/
{
    // set up lookat, perspective, and NDC-to-viewport transforms
    sPCam->SetLookAtMatrix();
    sSWPipe->SetViewportMatrix(sViewportX, sViewportY, sViewportWidth, sViewportHeight);

    sSWPipe->SetMatrixMode(GFX_PROJECTION);
    sSWPipe->LoadIdentity();
    sSWPipe->MultMatrix(sPCam->GetProjectionMatrix());	// CTM for projection stack is perspective transform

    // compute view-frame frustum plane equations for culling
    sSWPipe->SetFrustum(sSWPipe->GetClipper()->ComputeFrustum(sPCam->GetProjectionMatrix()));

    // enable lighting, gourard shading, depth buffering, ...
    sSWPipe->EnableLighting(false);
    sSWPipe->EnableShadows(false);
    sSWPipe->EnableDepthTest(true);
    sSWPipe->SetShadeMode(GFX_SMOOTH);
    sSWPipe->SetRenderMode(sWireframe ? GFX_WIREFRAME : GFX_FILLED);
    sSWPipe->SetRenderColor(0.f, 0.f, 0.f, 1.f); // use black color for wireframe
    sSWPipe->SetInterpMode(sInterpMode);

    sSWPipe->SetMatrixMode(GFX_MODELVIEW);
    sSWPipe->LoadMatrix(sPCam->GetLookAtMatrix());	// CTM for model-view stack is view matrix
    sSWPipe->SetLightPos(sScene[sLightIdx].GetWorldPosition());
    sSWPipe->SetShadowPlane(sReceiverPlane, sPCam->GetPosition());
    // Start processing primitives rendering.
    sSWPipe->RenderBegin();
    // first, render receiver as usual (with depth buffering, lighting (if lit))
    sScene[sReceiverIdx].Draw(sSWPipe);
    // second, render occluders' shadows above receiver by disabling depth buffering, 
  // lighting, texture mapping, with linear interpolation mode
    sSWPipe->EnableLighting(false);
    sSWPipe->EnableShadows(true);
    sSWPipe->EnableDepthTest(false);
    sSWPipe->SetInterpMode(GFX_LINEAR_INTERP);
    for (int i = 0; i < static_cast<int>(sScene.size()); ++i) {
        if (sScene[i].GetName() == "occluder") {
            sScene[i].Draw(sSWPipe);
        }
    }

    // finally, render rest of scene with lighting and depth buffering enabled,
    // while shadowing is disabled with the user selected interpolation mode 
    sSWPipe->SetInterpMode(sInterpMode);
    sSWPipe->EnableLighting(true);
    sSWPipe->EnableShadows(false);
    sSWPipe->EnableDepthTest(true);
    for (int i = 0; i < static_cast<int>(sScene.size()); ++i) {
        if (i != sReceiverIdx) {
            sScene[i].Draw(sSWPipe);
        }
    }

    sSWPipe->DrawText(0, 0, fmt.str()); // Draw the text.     

    sSWPipe->RenderEnd(); // Finish rendering and present the frame.
}

/*  _________________________________________________________________________ */
void InitScene()
/*!
        Initializes viewing parameters
        Loads models from IFS files.
        Instantiates objects from models into scene
*/
{
    double cx = 0.0, cy = 0.0;
    if (GLHelper::ptr_window) {
        glfwGetCursorPos(GLHelper::ptr_window, &cx, &cy); // Get initial cursor position.
    }
    sMX = cx;
    sMY = cy;

    // Load meshes. The second parameter specifies whether or
    // not to generate normals. Since IFS models have no colors
    // or textures, doing so is the only way we can get nice models.
    sCubeModelP = new gfxModel(gfxMeshImporter_IFS::Import("../content/cube.ifs", true));
    sSphereHRMP = new gfxModel(gfxMeshImporter_IFS::Import("../content/sphere_1010.ifs", true));
    sSphereLRMP = new gfxModel(gfxMeshImporter_IFS::Import("../content/sphere_0505.ifs", true));
    sGndModelP = new gfxModel(gfxMeshGenerator_Plane::Generate(L"../content/floor.png", 20, 200.0f));

    // create scene ...
    gfxVector3 u(1.f, 1.f, 1.f);
    gfxVector3 up(0.f, 1.f, 0.f);
    gfxVector3 w(u ^ up);
    gfxVector3 v(w ^ u);
    u.Normalize();
    v.Normalize();
    w.Normalize();
    gfxVector3 orient[3] = { u, v, w };
    gfxVector3 noRotation[3] = { gfxVector3(1.f, 0.f, 0.f),
                                                                gfxVector3(0.f, 1.f, 0.f),
                                                                gfxVector3(0.f, 0.f, 1.f) };

    // light source
    gfxObject	lightObj("light",
        sCubeModelP,
        gfxTransform(gfxVector3(1.f, 1.f, 1.f),
            noRotation,
            gfxVector3(5.f * sin(sAngle), 10.f, -5.f + 5.f * cos(sAngle))),
        false);
    lightObj.SetDiffuseMat(1.f, 0.f, 0.f);
    sLightIdx = static_cast<unsigned int>(sScene.size());
    sScene.push_back(lightObj);

    // occluders
    gfxObject	occObj1("occluder",
        sSphereLRMP,
        gfxTransform(gfxVector3(1.5f, 1.5f, 1.5f),
            noRotation,
            gfxVector3(0.f, 3.f, -5.f)),
        true);
    occObj1.SetDiffuseMat(0.f, 1.f, 0.f);
    sScene.push_back(occObj1);

    gfxObject	occObj2("occluder",
        sSphereLRMP,
        gfxTransform(gfxVector3(1.5f, 1.5f, 1.5f),
            noRotation,
            gfxVector3(-4.5f, 3.f, -5.f)),
        true);
    occObj2.SetDiffuseMat(0.f, 1.f, 1.f);
    sScene.push_back(occObj2);

    gfxObject	occObj3("occluder",
        sSphereLRMP,
        gfxTransform(gfxVector3(1.5f, 1.5f, 1.5f),
            noRotation,
            gfxVector3(4.5f, 3.f, -5.f)),
        true);
    occObj3.SetDiffuseMat(1.f, 0.f, 0.f);
    sScene.push_back(occObj3);

    gfxObject	occObj4("occluder",
        sCubeModelP,
        gfxTransform(gfxVector3(2.f, 1.f, 1.f),
            orient,
            gfxVector3(-8.5f, 3.f, -5.f)),
        true);
    occObj4.SetDiffuseMat(1.f, 0.f, 0.f);
    sScene.push_back(occObj4);


    gfxVector3	pillarScale(2.f, 2.f, 2.f);
    gfxVector3	leftPillarPos(-20.f, 1.f, 0.f);
    gfxVector3	midPillarPos(0.f, 1.f, 0.f);
    gfxVector3	rightPillarPos(20.f, 1.f, 0.f);
    gfxVector3	ballScale(2.f, 2.f, 2.f);

    // next, set up parallelepiped and spherical pillars - from back to front
    for (int z = -40, idx = 0; z <= 40; z += 8, ++idx) {
        // left parallelepiped pillar
        leftPillarPos.z = static_cast<float>(z);
        gfxObject leftPillar("occluder",
            sCubeModelP,
            gfxTransform(pillarScale, noRotation, leftPillarPos),
            true
        );
        leftPillar.SetDiffuseMat(1.f, 0.f, 0.f);
        sScene.push_back(leftPillar);

        // left spherical pillar
        float	disp = -45.f + ((idx % 2) ? 0.f : 10.f);
        gfxObject leftBall("occluder",
            sSphereLRMP,
            gfxTransform(ballScale, noRotation, gfxVector3(disp, 1.75f, static_cast<float>(z))),
            true
        );
        leftBall.SetDiffuseMat(0.f, 0.f, 1.f);
        sScene.push_back(leftBall);

        midPillarPos.z = static_cast<float>(z);
        gfxObject midPillar("occluder",
            sCubeModelP,
            gfxTransform(pillarScale, noRotation, midPillarPos),
            true
        );
        midPillar.SetDiffuseMat(0.f, 1.f, 0.f);
        sScene.push_back(midPillar);

        rightPillarPos.z = static_cast<float>(z);
        gfxObject	rightPillar("occluder",
            sCubeModelP,
            gfxTransform(pillarScale, noRotation, rightPillarPos),
            true
        );
        rightPillar.SetDiffuseMat(0.f, 0.f, 1.f);
        sScene.push_back(rightPillar);

        disp = 45.f + ((idx % 2) ? 10.f : 0.f);
        gfxObject	rightBall("occluder",
            sSphereLRMP,
            gfxTransform(ballScale, noRotation, gfxVector3(disp, 1.75f, static_cast<float>(z))),
            true
        );
        rightBall.SetDiffuseMat(1.f, 0.f, 0.f);
        sScene.push_back(rightBall);
    }

    // finally, set up receiver plane
    gfxObject ground("receiver",
        sGndModelP,
        gfxTransform(gfxVector3(1.f, 1.f, 1.f), noRotation, gfxVector3(0.f, 0.f, 0.f))
    );
    sReceiverIdx = static_cast<unsigned int>(sScene.size());
    sScene.push_back(ground);
}

/*  _________________________________________________________________________ */
void HandleKey(int key, int action)
/*! Keyboard input handler -- GLFW key events. The handler
    BODY -- i.e. what each key does to the camera/scene -- is unchanged;
    only the *source* of input changed (GLFW window instead of
    gfxGraphicsPipe's now-hidden Win32 window).

    @param key    -->  GLFW key token (GLFW_KEY_*)
    @param action -->  GLFW_PRESS or GLFW_REPEAT
*/
{
    (void)action;
    switch (key) {
        // E and D zoom the camera in or out.
    case GLFW_KEY_E: sPCam->Move(0.f, 0.f, sCamStep); break;
    case GLFW_KEY_D: sPCam->Move(0.f, 0.f, -sCamStep); break;

        // S and F pan the camera right or left.
    case GLFW_KEY_S: sPCam->Move(sCamStep, 0.f, 0.f); break;
    case GLFW_KEY_F: sPCam->Move(-sCamStep, 0.f, 0.f); break;

        // Q and A pan the camera up or down.
    case GLFW_KEY_Q: sPCam->Move(0.f, sCamStep, 0.f); break;
    case GLFW_KEY_A: sPCam->Move(0.f, -sCamStep, 0.f); break;

        // W and R yaw the camera.
    case GLFW_KEY_W: sPCam->SetAzimuth(sPCam->GetAzimuth() + sCamStep / 3.f); break;
    case GLFW_KEY_R: sPCam->SetAzimuth(sPCam->GetAzimuth() - sCamStep / 3.f); break;

        // T and G pitch the camera..
    case GLFW_KEY_T: sPCam->SetLatitude(sPCam->GetLatitude() + sCamStep / 3.f); break;
    case GLFW_KEY_G: sPCam->SetLatitude(sPCam->GetLatitude() - sCamStep / 3.f); break;

        // Space toggles mouselook.
    case GLFW_KEY_SPACE: sMouseLook = !sMouseLook; sPickID = -1; break;

        // V toggles wireframe.
    case GLFW_KEY_V: sWireframe = !sWireframe; break;

        // H toggles interpolation mode between hyperbolic and linear
    case GLFW_KEY_H:
        sInterpMode = (sInterpMode == GFX_LINEAR_INTERP) ? GFX_HYPERBOLIC_INTERP : GFX_LINEAR_INTERP;
        break;

        // Escape resets the camera (window-close is handled separately, see main loop).
    case GLFW_KEY_ESCAPE: {
        sPCam->SetPosition(sCameraPosition);
        sPCam->SetTarget(sCameraTarget);
    }
                  break;
    default: {
        // Do nothing.
    }
    }
}

/*  _________________________________________________________________________ */
void HandleMouseButton(int button, int action, double x, double y)
/*! Mouse input handler -- GLFW mouse-button events. The
    handler BODY is unchanged, including reuse of gfxGraphicsPipe's own
    picker (sSWPipe->GetPicker()->Pick(...)) -- the CPU rasterizer is alive
    and rendering, so its picking facility works exactly as before.

    @param button -->  GLFW_MOUSE_BUTTON_LEFT / GLFW_MOUSE_BUTTON_RIGHT
    @param action -->  GLFW_PRESS or GLFW_RELEASE
    @param x, y   -->  cursor position (window client-area coordinates)
*/
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            sMoveF = true;
            sMoveB = false;

            if (!sMouseLook) {
                sPickID = sSWPipe->GetPicker()->Pick(static_cast<int>(x), static_cast<int>(y), sScene, sPCam);
            }
        }
        else if (action == GLFW_RELEASE) {
            sMoveF = false;
            sMoveB = false;
            sPickID = -1;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            sMoveF = false;
            sMoveB = true;
        }
        else if (action == GLFW_RELEASE) {
            sMoveF = false;
            sMoveB = false;
        }
    }
}

/*  _________________________________________________________________________ */
void HandleMousePos(double x, double y)
{
    (void)x; (void)y;
}

class FPSCounter {
public:
    void Start() {
        // Initialize FPS counter
        frameCount = 0;
        startTime = std::chrono::steady_clock::now();
    }

    double FPS() {
        return fps;
    }

    void Frame() {
        // Count each frame
        ++frameCount;

        // Calculate elapsed time since last FPS update
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

        if (elapsedTime >= 1000) { // Update FPS every 1 second
            // Calculate FPS
            fps = frameCount / (elapsedTime / 1000.0);

            // Print FPS
            //std::cout << "FPS: " << fps << std::endl;

            // Reset counters for the next second
            frameCount = 0;
            startTime = currentTime;
        }
    }

private:
    int frameCount;
    double fps = 0;
    std::chrono::steady_clock::time_point startTime;
};

/*  _________________________________________________________________________ */
int main()
/*! Application entry point.

    Mirrors assignment-2's main-pbo.cpp game-loop structure
    (init -> while(!glfwWindowShouldClose) { update(); draw(); } -> cleanup),
    while preserving virtually all of the original WinMain's scene
    setup/update/draw logic, which continues to be driven entirely by
    gfxGraphicsPipe (our CPU software rasterizer).

    @return
    A result code.
*/
{
    // Load GDI+. gfxMeshGenerator_Plane::Generate (used by InitScene to build
    // the textured ground/load floor.png) relies on GDI+ being initialized.
    ULONG_PTR                     gdi = 0;
    Gdiplus::GdiplusStartupInput  gdiInput(0, 0, 0);
    Gdiplus::GdiplusStartup(&gdi, &gdiInput, 0);

    // Initialize the cycle-resolution timer.
    gfxCycleTimer::Init();

    // Create the visible presentation window + OpenGL context (GLFW/GLEW).
    if (!GLHelper::init(sWindowWidth, sWindowHeight, "CSD2101 AY2526 T3 Project")) {
        std::cerr << "Unable to create OpenGL context" << std::endl;
        Gdiplus::GdiplusShutdown(gdi);
        return EXIT_FAILURE;
    }

    // Route GLFW input events to the original (re-homed) handler bodies.
    GLHelper::on_key = &HandleKey;
    GLHelper::on_mousebutton = &HandleMouseButton;
    GLHelper::on_mousepos = &HandleMousePos;

    // Create custom implementation objects.
#ifdef YOUR_RASTERIZER
    gYourRasterizer = new YourRasterizer();
#endif
#ifdef YOUR_CULLER_CLIPPER
    sYourClipper = new YourClipper();
#endif

    // initialize camera
    sPCam->SetPosition(sCameraPosition);
    sPCam->SetTarget(sCameraTarget);
    sPCam->SetUp(sCameraUp);
    sPCam->SetAspect(sAspectRatio);
    sPCam->SetFOV(sFOVY);
    sPCam->SetNearDist(sNearDist);
    sPCam->SetFarDist(sFarDist);
    sPCam->SetViewport(sViewportX, sViewportY, sViewportWidth, sViewportHeight);
    sPCam->SetPerspMatrix(sFOVY, sAspectRatio, sNearDist, sFarDist);

    // Create and initialize the CPU software rasterizer (our "GLPbo").
    // gfxGraphicsPipe mandatorily creates and owns its own Win32 window
    // internally (it's precompiled -- there's no source to redirect this).
    // We hide that window immediately: the user only ever sees the GLFW/GL
    // window, which displays the pipe's CPU-rasterized framebuffer via
    // BlitQuad's single pass-through shader.
    sSWPipe = new gfxGraphicsPipe(sWindowX, sWindowY,
        sWindowWidth, sWindowHeight,
        "CSD2101 AY2526 T3 Project (CPU raster surface - hidden)");
    ::ShowWindow(sSWPipe->GetHWND(), SW_HIDE);

    // Now that both the GL context and the CPU rasterizer (which determines
    // framebuffer dimensions) exist, set up the GPU-side display: one
    // texture sized to match, the full-window quad, and the single
    // pass-through blit shader.
    sBlitQuad.init(static_cast<GLsizei>(sSWPipe->GetWidth()), static_cast<GLsizei>(sSWPipe->GetHeight()));

    int result = EXIT_SUCCESS;
    try {
        // Hook up the controller overrides.
#ifdef YOUR_RASTERIZER
        sSWPipe->UseRasterizer(gYourRasterizer);
#else
        sSWPipe->UseRasterizer(0);
#endif

#ifdef YOUR_CULLER_CLIPPER
        sSWPipe->UseClipper(sYourClipper);
#else
        sSWPipe->UseClipper(0);
#endif

        sSWPipe->UseLighter(0);
        sSWPipe->UsePicker(0);

        InitScene();
        std::cout << "Set up complete\n";
        std::stringstream tmp_ss("");
        DrawScene(tmp_ss);

        FPSCounter fpsCounter;
        fpsCounter.Start();

        double accumulator = 0.0;
        double const frameDt = 1.0 / static_cast<double>(sMaxFps);

        while (!glfwWindowShouldClose(GLHelper::ptr_window)) {
            glfwPollEvents(); // also pumps gfxGraphicsPipe's hidden window's message queue

            GLHelper::update_time(1.0);
            accumulator += GLHelper::delta_time;

            // throttle game to constant frame rate of sMaxFps frames/second
            if (accumulator >= frameDt) {
                accumulator = 0.0;

                UpdateScene();

                std::stringstream  fmt;
                fmt.precision(2);
                fmt << "Mouselook " << (sMouseLook ? "ON" : "OFF") << " Object Cnt: " << sScene.size()
                    << "; Culled: " << sSWPipe->GetCullCount() << "; FPS: " << fpsCounter.FPS() << "; sPickID: " << sPickID;

                // 1) CPU-rasterize the entire frame (gfxGraphicsPipe does all
                //    the work -- transform/lighting/shading/shadows/text --
                //    exactly like GLPbo::emulate()).
                DrawScene(fmt);

                // 2) Display: upload the CPU framebuffer to a GL texture and
                //    blit it full-window with the single pass-through shader
                //    (mirrors GLPbo::draw_fullwindow_quad()).
                sBlitQuad.update_and_draw(sSWPipe->GetFrameBuffer());
                glfwSwapBuffers(GLHelper::ptr_window);

                fpsCounter.Frame();
            }
        }
    }
    catch (std::exception const& e) {
        ::MessageBox(0, e.what(), "Error", MB_OK);
        result = EXIT_FAILURE;
    }

    // Exit.
    sBlitQuad.cleanup();
    GLHelper::cleanup();

    Gdiplus::GdiplusShutdown(gdi);
    ::ReleaseCapture();

#ifdef YOUR_CULLER_CLIPPER
    delete sYourClipper;
#endif
#ifdef YOUR_RASTERIZER
    delete gYourRasterizer;
#endif

    delete	sSWPipe;
    delete	sPCam;
    delete	sCubeModelP;
    delete	sSphereLRMP;
    delete	sSphereHRMP;
    delete	sGndModelP;

    return (result);
}
