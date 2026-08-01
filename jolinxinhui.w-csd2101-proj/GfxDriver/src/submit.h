/*!
@file    submit.h
@author  Prasanna Ghali       (pghali@digipen.edu)
         Josh Petrie          (jmp, jpetrie@digipen.edu)
         Scott Smith          (sls, ssmith@digipen.edu)
         Patrick Laukaitis    (pjl, plaukait@digipen.edu)
@co-author  Parminder Singh   (parminder.singh@digipen.edu)

Consolidates the declarations of the classes you must implement --
YourCamera, YourClipper, and YourRasterizer (previously YourCamera.h,
YourCullerClipper.h, and YourRasterizer.h) -- so that all your work is
submitted in a single file pair: submit.h / submit.cpp.

All content (c) 2002 DigiPen Institute of Technology, all rights reserved.
*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */

#ifndef SUBMIT_H_
#define SUBMIT_H_

/*                                                                   includes
----------------------------------------------------------------------------- */

#include  "../../GfxLib/gfx/GFX.h"

/*                                                                    classes
----------------------------------------------------------------------------- */

/*  _________________________________________________________________________ */
class YourCamera : public gfxCamera
/*! Example camera wrapper class.

    This class contains the basic methods for a spherical camera interface that
    wraps the quaternion-based camera used internally by the device. All
    methods are empty and the class has no data other than a pointer to the
    actual camera that is being wrapped. You can manipulate the actual camera
    with the Update() method, which takes the eye position and target position.
    You don't need to deal with support camera roll (e.g., altering up vector).

    You are expected to store or compute, as appropriate, the camera's position,
    and view vector using spherical coordinates.
*/
{
  public:
    // ct and dt
             YourCamera() { }
    virtual ~YourCamera() { }

    // displacement
    virtual void Move(float x, float y, float z);
    virtual void Move(gfxVector3 const& p);

    // updates
    virtual void UpdateSphericalFromPoints();
    virtual void UpdatePointsFromSpherical();
};

/*  _________________________________________________________________________ */
class YourClipper : public gfxController_Clipping
/*! Example clipper subclass.

    This class implements culling (by bounding spheres) and clipping.
    You need to implement both the Cull() and the Clip() functions. More details
    can be found in the comments above each function.
*/
{
  public:
    // ct and dt
             YourClipper() { }
    virtual ~YourClipper() { }

		// operation - compute view-frame frustum
		virtual gfxFrustum ComputeFrustum(gfxMatrix4 const&	projection_mtx);

    // operations - culling
    virtual bool Cull(gfxSphere const&	bounding_sphere,
                      gfxFrustum const&	frustum,
                      gfxOutCode        *ptr_outcode);

    // operations - clipping
    virtual gfxVertexBuffer Clip(gfxOutCode outcode, gfxVertexBuffer const& vertex_buffer);
};

/*  _________________________________________________________________________ */
class YourRasterizer : public gfxController_Rasterization
/*! Example rasterizer subclass.

    An instance of this subclass can be created and used to override the
    reference rasterizer in a specific pipe by calling the UseRasterizer()
    method of gfxGraphicsPipe.

    You can set the render mode of a pipe by calling SetRenderMode() and
    passing either GFX_POINTS, GFX_WIREFRAME, or GFX_FILLED. The appropriate
    function from this rasterizer subclass will be invoked for each triangle.

    Within each function, dev points to the pipe to which the triangle should
    be rasterized. You can get a pointer to the framebuffer and its width and
    height from there. Each vertex contains x_d and y_d fields (X and Y screen
    position) as well as an z_d field (depth value), r, g, b color fields, and
		u, v texture coordinate fields.

    The pipe framebuffer is an array of unsigned ints in ARGB format. The
    examples below should be enough to get you started.
*/
{
  public:
    // ct and dt
             YourRasterizer() { }
    virtual ~YourRasterizer() { }

    // operations
    virtual void DrawPoint(gfxGraphicsPipe*, gfxVertex const&);
		virtual void DrawLine(gfxGraphicsPipe*, gfxVertex const&, gfxVertex const&);
		virtual void DrawWireframe(gfxGraphicsPipe*, gfxVertex const&, gfxVertex const&, gfxVertex const&);
		virtual void DrawFilled(gfxGraphicsPipe*, gfxVertex const&, gfxVertex const&, gfxVertex const&);
};

#endif  /* SUBMIT_H_ */
