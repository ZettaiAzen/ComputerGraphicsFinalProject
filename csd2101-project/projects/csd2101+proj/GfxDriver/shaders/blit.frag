#version 450 core

layout (location=0) in vec2 vTexCoord;

layout (location=0) out vec4 fFragClr;

uniform sampler2D uTex0;

void main() {
fFragClr = texture(uTex0, vTexCoord);
}
