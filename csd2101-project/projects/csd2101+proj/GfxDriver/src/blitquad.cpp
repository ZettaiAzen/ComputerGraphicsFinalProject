/*!
@file    blitquad.cpp
@author  parminder.singh@digipen.edu

See blitquad.h. Implementation closely mirrors GLPbo::setup_quad_vao /
setup_shdrpgm / draw_fullwindow_quad / init / cleanup in
assignment-2/projects/assignment-2/src/glpbo.cpp.
*//*__________________________________________________________________________*/
#include <blitquad.h>
#include <glm/glm.hpp>
#include <array>
#include <iostream>
#include <cstdlib>

/*  _________________________________________________________________________ */
/*! setup_quad_vao  (mirrors GLPbo::setup_quad_vao, glpbo.cpp ~919-955)

Builds the geometry for a full-window rectangle made of 2 triangles.

NOTE on texture-coordinate orientation: gfxGraphicsPipe::GetFrameBuffer()
documents its pixel buffer with the origin (index 0) at the buffer's
UPPER-LEFT corner (row-major, top-down) -- see submit.cpp's DrawFilled() header
comment ("frameBuf[0] will paint the upper-left corner ... xxRRGGBB").
glTextureSubImage2D treats the first row of the supplied data as the
texture's row 0, which corresponds to texture coordinate t = 0 (the texture's
"bottom" in the standard OpenGL sampling convention). To present the image
right-side-up we therefore use a vertically-flipped st mapping relative to
GLPbo's (whose own PBO buffer is bottom-up): the screen's top-left corner
samples t = 0 (texture row 0 == the framebuffer's top row).
*/
void BlitQuad::setup_quad_vao() {
  std::array<glm::vec2, 4> pos_vtx{
    glm::vec2(1.f, -1.f), glm::vec2(1.f, 1.f), glm::vec2(-1.f, 1.f), glm::vec2(-1.f, -1.f)
  };
  std::array<glm::vec2, 4> st_vtx{
    glm::vec2(1.f, 1.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 1.f)
  };

  GLuint vbo_hdl;
  glCreateBuffers(1, &vbo_hdl);
  glNamedBufferStorage(vbo_hdl, 2 * sizeof(glm::vec2) * pos_vtx.size(),
    nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
  glNamedBufferSubData(vbo_hdl, 0, sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
  glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(),
    sizeof(glm::vec2) * st_vtx.size(), st_vtx.data());

  glCreateVertexArrays(1, &vaoid);
  glEnableVertexArrayAttrib(vaoid, 0);
  glEnableVertexArrayAttrib(vaoid, 1);
  glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(glm::vec2));
  glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec2));
  glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(vaoid, 1, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(vaoid, 0, 0);
  glVertexArrayAttribBinding(vaoid, 1, 1);

  std::array<GLushort, 6> idx_vtx{ 0, 1, 2, 2, 3, 0 };
  GLuint ebo_hdl;
  glCreateBuffers(1, &ebo_hdl);
  glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(),
    reinterpret_cast<GLvoid*>(idx_vtx.data()), GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
  glVertexArrayElementBuffer(vaoid, ebo_hdl);
  elem_cnt = 6;
}

/*  _________________________________________________________________________ */
/*! setup_shdrpgm

Loads the *single* shader program used by this entire application: a trivial
pass-through vertex shader plus a fragment shader that just samples a
texture -- byte-for-byte the same pair used by GLPbo
(see GLPbo::setup_shdrpgm, glpbo.cpp:967-1002), just stored as files instead
of inline strings.
*/
void BlitQuad::setup_shdrpgm() {
  if (!shdr_pgm.CompileShaderFromFile(GL_VERTEX_SHADER, "shaders/blit.vert") ||
      !shdr_pgm.CompileShaderFromFile(GL_FRAGMENT_SHADER, "shaders/blit.frag")) {
    std::cout << "Blit shader compile failed: " << shdr_pgm.GetLog() << std::endl;
    std::exit(EXIT_FAILURE);
  }
  shdr_pgm.Link();
  shdr_pgm.Validate();
  if (GL_FALSE == shdr_pgm.IsLinked()) {
    std::cout << "Unable to compile/link/validate blit shader program: "
      << shdr_pgm.GetLog() << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

/*  _________________________________________________________________________ */
/*! init  (mirrors the texture/PBO-presentation half of GLPbo::init)

Allocates a GL_RGBA8 texture sized to match gfxGraphicsPipe's framebuffer,
the full-window quad, and compiles/links the single blit shader program.
*/
void BlitQuad::init(GLsizei w, GLsizei h) {
  width = w;
  height = h;

  glCreateTextures(GL_TEXTURE_2D, 1, &texid);
  glTextureStorage2D(texid, 1, GL_RGBA8, width, height);
  glTextureParameteri(texid, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(texid, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTextureParameteri(texid, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(texid, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  setup_quad_vao();
  setup_shdrpgm();
}

/*  _________________________________________________________________________ */
/*! update_and_draw  (mirrors the tail of GLPbo::emulate + GLPbo::draw_fullwindow_quad)

Uploads the CPU-rasterized pixel buffer to the GL texture
(glTextureSubImage2D, exactly like glpbo.cpp:449) then draws the full-window
quad textured with it, using the single pass-through blit shader
(glpbo.cpp:1005-1014).

`pixels` is gfxGraphicsPipe's xxRRGGBB-packed buffer (see
submit.cpp's DrawFilled() documentation comment). On a little-endian machine the
in-memory byte order of 0x00RRGGBB is B,G,R,X -- i.e. exactly GL_BGRA -- so
we upload with format = GL_BGRA. The unused top byte (alpha) doesn't matter:
we never enable blending for the blit, so the framebuffer simply displays RGB.
*/
void BlitQuad::update_and_draw(unsigned int const* pixels) {
  glTextureSubImage2D(texid, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

  shdr_pgm.Use();
  shdr_pgm.SetUniform("uTex0", 0);
  glBindTextureUnit(0, texid);
  glBindVertexArray(vaoid);
  glDrawElements(GL_TRIANGLES, elem_cnt, GL_UNSIGNED_SHORT, NULL);
  glBindVertexArray(0);
  glBindTextureUnit(0, 0);
  shdr_pgm.UnUse();
}

/*  _________________________________________________________________________ */
void BlitQuad::cleanup() {
  glDeleteVertexArrays(1, &vaoid);
  glDeleteTextures(1, &texid);
  shdr_pgm.DeleteShaderProgram();
}
