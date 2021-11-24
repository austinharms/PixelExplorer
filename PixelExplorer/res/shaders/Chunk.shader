#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
out vec2 v_TexCoord;
uniform mat4 u_MVP;

void main() {
  gl_Position = u_MVP * vec4(position, 1.0);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;
uniform sampler2D u_Texture;

void main() { 
  vec4 texColor;
  texColor = texture2D(u_Texture, v_TexCoord);
  color = texColor;
};
