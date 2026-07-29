/*!
@file    blitquad.h
@author  parminder.singh@digipen.edu

Encapsulates the *only* GPU-side rendering project `p` performs: displaying
a CPU-rasterized pixel buffer (produced each frame by gfxGraphicsPipe, our
software rasterizer -- exactly like assignment-2's GLPbo) onscreen.

This mirrors GLPbo::setup_quad_vao / setup_shdrpgm / draw_fullwindow_quad /
init / cleanup (see assignment-2/projects/assignment-2/src/glpbo.cpp,
roughly lines 256-300 and 967-1021):
  - one GL_RGBA8 texture, updated every frame via glTextureSubImage2D from
    the CPU framebuffer (gfxGraphicsPipe::GetFrameBuffer())
  - one full-screen quad (VAO/VBO/EBO, 2 triangles via 6 indices)
  - exactly ONE shader program: a trivial pass-through vertex shader plus a
    fragment shader that just samples the texture (blit.vert / blit.frag,
    loaded from the `shaders/` directory copied next to the executable)

*//*__________________________________________________________________________*/
#ifndef BLITQUAD_H
#define BLITQUAD_H

#include <glslshader.h>
#include <cstddef>

class BlitQuad {
public:
  // w, h: dimensions of the CPU framebuffer (== gfxGraphicsPipe's window size)
  void init(GLsizei w, GLsizei h);
  void cleanup();

  // Uploads `pixels` (an RGBA-packed buffer, w*h 32-bit values, row 0 = TOP
  // row of the image -- matching gfxGraphicsPipe::GetFrameBuffer()'s
  // documented xxRRGGBB / top-left-origin layout) to the GL texture and
  // draws the full-window quad with the single pass-through blit shader.
  void update_and_draw(unsigned int const* pixels);

private:
  GLSLShader   shdr_pgm;
  GLuint       vaoid = 0;
  GLuint       texid = 0;
  GLsizei      width = 0;
  GLsizei      height = 0;
  GLsizei      elem_cnt = 0;

  void setup_quad_vao();
  void setup_shdrpgm();
};

#endif // BLITQUAD_H
