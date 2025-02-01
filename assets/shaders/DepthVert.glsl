#version 460 core
			
layout(location = 0) in vec3 a_vertexPosition;

layout (std140, binding = 4) uniform b_shadowCamera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform mat4 u_viewPos;
};

uniform mat4 u_model;


void main()
{

	gl_Position = u_projection * u_view * u_model * vec4(a_vertexPosition, 1.0);



}