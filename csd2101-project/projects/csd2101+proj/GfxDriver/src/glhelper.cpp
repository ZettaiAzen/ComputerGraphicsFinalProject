/*!
@file    	  glhelper.cpp
@author  	  pghali@digipen.edu
@co-author	  parminder.singh@digipen.edu

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and use GLEW to extract function
pointers to OpenGL implementations.

Trimmed for project `p` (no course-automation hooks). GLHelper owns only the
*visible* presentation window/context; gfxGraphicsPipe (hidden window) keeps
doing 100% of the scene rendering on the CPU, exactly like assignment-2's
GLPbo -- the only thing that changed is how the resulting pixel buffer is
displayed (OpenGL blit-quad instead of Win32/GDI BitBlt).
*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glhelper.h>
#include <iostream>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
// static data members declared in GLHelper
GLint GLHelper::width;
GLint GLHelper::height;
GLdouble GLHelper::fps;
GLdouble GLHelper::delta_time;
std::string GLHelper::title;
GLFWwindow* GLHelper::ptr_window;

std::function<void(int, int)>                       GLHelper::on_key;
std::function<void(int, int, double, double)>       GLHelper::on_mousebutton;
std::function<void(double, double)>                 GLHelper::on_mousepos;

/*  _________________________________________________________________________ */
/*! init

@param GLint width
@param GLint height
Dimensions of window requested by program

@param std::string title_str
String printed to window's title bar

@return bool
true if OpenGL context and GLEW were successfully initialized.
false otherwise.

Uses GLFW to create OpenGL context: a window of size width x height pixels
and its associated OpenGL context that matches a core profile that is
compatible with OpenGL 4.5, has 32-bit RGBA, double-buffered color buffer,
24-bit depth buffer and 8-bit stencil buffer.
*/
bool GLHelper::init(GLint width, GLint height, std::string title) {
  GLHelper::width = width;
  GLHelper::height = height;
  GLHelper::title = title;

  // Part 1
  if (!glfwInit()) {
    std::cout << "GLFW init has failed - abort program!!!" << std::endl;
    return false;
  }

  // In case a GLFW function fails, an error is reported to callback function
  glfwSetErrorCallback(GLHelper::error_cb);

  // Before asking GLFW to create an OpenGL context, we specify the minimum
  // constraints in that context:
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // window dimensions are static

  GLHelper::ptr_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (!GLHelper::ptr_window) {
    std::cerr << "GLFW unable to create OpenGL context - abort program\n";
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(GLHelper::ptr_window);

  setup_event_callbacks();

  // this is the default setting ...
  glfwSetInputMode(GLHelper::ptr_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  // Part 2: Initialize entry points to OpenGL functions and extensions
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::cerr << "Unable to initialize GLEW - error: "
      << glewGetErrorString(err) << " abort program" << std::endl;
    return false;
  }
  if (GLEW_VERSION_4_5) {
    std::cout << "Using glew version: " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Driver supports OpenGL 4.5\n" << std::endl;
  } else {
    std::cerr << "Warning: The driver may lack full compatibility with OpenGL 4.5, potentially limiting access to advanced features." << std::endl;
  }

  return true;
}

void GLHelper::setup_event_callbacks() {
    glfwSetFramebufferSizeCallback(GLHelper::ptr_window, GLHelper::fbsize_cb);
    glfwSetKeyCallback(GLHelper::ptr_window, GLHelper::key_cb);
    glfwSetMouseButtonCallback(GLHelper::ptr_window, GLHelper::mousebutton_cb);
    glfwSetCursorPosCallback(GLHelper::ptr_window, GLHelper::mousepos_cb);
    glfwSetScrollCallback(GLHelper::ptr_window, GLHelper::mousescroll_cb);
}

/*  _________________________________________________________________________ */
/*! cleanup

Returns resources to the system through GLFW and gracefully terminates.
*/
void GLHelper::cleanup() {
  glfwTerminate();
}

/*  _________________________________________________________________________*/
/*! key_cb

Forwards the raw GLFW key event to main.cpp's registered handler
(GLHelper::on_key), which contains the original MyKeyHandler logic re-homed
from Win32 WM_KEYDOWN messages to GLFW key events. Also handles ESC to close
the window, mirroring the original camera-reset + standard close behavior.
*/
void GLHelper::key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod) {
  (void)scancode; (void)mod;
  if (on_key && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    on_key(key, action);
  }
}

/*  _________________________________________________________________________*/
/*! mousebutton_cb

Forwards raw GLFW mouse-button events (with current cursor position) to
main.cpp's registered handler -- re-homing the original MyMouseHandler logic
from Win32 WM_LBUTTONDOWN/UP/RBUTTONDOWN/UP messages to GLFW mouse events.
*/
void GLHelper::mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod) {
  (void)mod;
  if (on_mousebutton) {
    double x, y;
    glfwGetCursorPos(pwin, &x, &y);
    on_mousebutton(button, action, x, y);
  }
}

/*  _________________________________________________________________________*/
/*! mousepos_cb

Forwards raw GLFW cursor-position events to main.cpp's registered handler.
*/
void GLHelper::mousepos_cb(GLFWwindow *pwin, double xpos, double ypos) {
  (void)pwin;
  if (on_mousepos) {
    on_mousepos(xpos, ypos);
  }
}

/*  _________________________________________________________________________*/
/*! mousescroll_cb

Not used by project `p` -- kept for parity with assignment-2's GLHelper.
*/
void GLHelper::mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset) {
  (void)pwin; (void)xoffset; (void)yoffset;
}

/*  _________________________________________________________________________ */
/*! error_cb

The error callback receives a human-readable description of the error and
(when possible) its cause.
*/
void GLHelper::error_cb(int error, char const* description) {
  (void)error;
  std::cerr << "GLFW error: " << description << std::endl;
}

/*  _________________________________________________________________________ */
/*! fbsize_cb

Called when the window is resized -- receives the new size in pixels.
*/
void GLHelper::fbsize_cb(GLFWwindow *ptr_win, int width, int height) {
  (void)ptr_win;
  GLHelper::width = width;
  GLHelper::height = height;
}

/*  _________________________________________________________________________*/
/*! update_time

Must be called once per game loop. Uses GLFW's time functions to compute:
1. the interval in seconds between each frame
2. the frames per second every "fps_calc_interval" seconds
*/
void GLHelper::update_time(double fps_calc_interval) {
  // get elapsed time (in seconds) between previous and current frames
  static double prev_time = glfwGetTime();
  double curr_time = glfwGetTime();
  delta_time = curr_time - prev_time;
  prev_time = curr_time;

  // fps calculations
  static double count = 0.0; // number of game loop iterations
  static double start_time = glfwGetTime();
  // get elapsed time since very beginning (in seconds) ...
  double elapsed_time = curr_time - start_time;

  ++count;

  // update fps at least every 10 seconds ...
  fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
  fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;
  if (elapsed_time > fps_calc_interval) {
    GLHelper::fps = count / elapsed_time;
    start_time = curr_time;
    count = 0.0;
  }
}
