#version 460 core

layout(location = 0) in vec3 a_vertexPosition;
//layout(location = 1) in vec3 a_vertexNormal;
layout(location = 1) in vec2 a_texCoord;
//layout(location = 3) in vec3 a_tangent;

layout(std140, binding = 3) uniform b_cameraCol
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

out vec3 vs_fragmentPos;
out vec2 vs_texCoord;
//out vec3 vs_vertexNormal;
out vec3 vs_vertPos;

//out vec3 fragmentPos;
//out vec2 texCoord;
//out vec3 vertexNormal;


vec4 vertexPosition;
//out mat3 TBN;

uniform mat4 u_model;
uniform mat4 u_lightSpaceTransform;

void main()
{
	vec4 vertexPrePos = u_model * vec4(a_vertexPosition, 1.0);
	vertexPosition = vertexPrePos;// +vec4(0, (sin(a_vertexPosition.x * 0.7) * 0.5) + sin(a_vertexPosition.z * 0.3) * 0.5, 0, 0);
	vs_vertPos = vertexPosition.xyz;
	vs_fragmentPos = vec3(vertexPosition) - u_viewPos;

	vs_texCoord = a_texCoord;

}