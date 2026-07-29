/* !
@file    	glhelper.h
@author  	pghali@digipen.edu
@co-author  parminder.singh@digipen.edu

This file contains the declaration of namespace Helper that encapsulates the
functionality required to create an OpenGL context using GLFW and use GLEW
to load OpenGL extensions.

Trimmed for project `p`: this is the *presentation* layer only. All scene
rendering continues to be done by gfxGraphicsPipe (a CPU software rasterizer),
exactly as in assignment-2's GLPbo. GLHelper merely owns the visible window,
the GL context, and routes user input (key/mouse) to callbacks supplied by
main.cpp -- mirroring assignment-2's glhelper.h/.cpp, minus the
course-automation hooks (recorder/hook_csd2101) that aren't vendored here.
*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLHELPER_H
#define GLHELPER_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <GL/glew.h> // for access to OpenGL API declarations
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

/*  _________________________________________________________________________ */
struct GLHelper
  /*! GLHelper structure to encapsulate initialization stuff ...
  */
{
  static bool init(GLint width, GLint height, std::string title);
  static void cleanup();

  // callbacks ...
  static void error_cb(int error, char const* description);
  static void fbsize_cb(GLFWwindow *ptr_win, int width, int height);
  // I/O callbacks ...
  static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);
  static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);
  static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);
  static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos);
  // setup callbacks for events
  static void setup_event_callbacks();

  static void update_time(double fpsCalcInt = 1.0);

  // main.cpp registers these to receive raw GLFW input events; GLHelper's
  // own GLFW callbacks forward to them. This lets main.cpp keep its
  // original MyKeyHandler/MyMouseHandler logic bodies, just re-homed from
  // Win32 messages to GLFW events.
  static std::function<void(int key, int action)>                    on_key;
  static std::function<void(int button, int action, double x, double y)> on_mousebutton;
  static std::function<void(double x, double y)>                     on_mousepos;

  static GLint width, height;
  static GLdouble fps;
  static GLdouble delta_time; // time taken to complete most recent game loop
  static std::string title;
  static GLFWwindow *ptr_window;
};

#endif /* GLHELPER_H */
