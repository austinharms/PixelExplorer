#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec2 texRepeat;
out vec2 v_TexCoord;
out vec2 v_TexRepeat;
uniform mat4 u_MVP;

void main() {
  gl_Position = u_MVP * vec4(position, 1.0);
  v_TexCoord = texCoord;
  v_TexRepeat = texRepeat;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;
in vec2 v_TexRepeat;
uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main() { 
  vec2 pos = v_TexCoord;
  if (v_TexRepeat.x != 1.0f) {
    float tw = (1.0f / 96.0f) * 16.0f;
    pos.x = floor(pos.x / tw) * tw;
    pos.x += mod((v_TexCoord.x - pos.x) * v_TexRepeat.x, tw);
  }
    
  if (v_TexRepeat.y != 1.0f) {
    float th = (1.0f / 64.0f) * 16.0f;
    pos.y = floor(pos.y / th) * th;
    pos.y += mod((v_TexCoord.y - pos.y) * v_TexRepeat.y, th);
  }

  vec4 texColor = texture(u_Texture, pos);
  texColor *= u_Color;
  color = texColor;
};
