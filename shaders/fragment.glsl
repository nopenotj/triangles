
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D our_tex;
void main() {
  FragColor = texture(our_tex, TexCoord);
};
