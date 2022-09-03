#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewProjectionMatrix;
uniform vec4 u_Color;

out vec4 v_Color;

void main() {
	gl_Position = u_ViewProjectionMatrix * u_ModelMatrix * vec4(position, 1.0);
	v_Color = u_Color;
};

#shader fragment
#version 330 core

in vec4 v_Color;

layout(location = 0) out vec4 color;

void main() {
	color = v_Color;
};