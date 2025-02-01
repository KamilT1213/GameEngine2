#version 460 core
			
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec3 a_vertexNormal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;

layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

out vec3 fragmentPos;
out vec2 texCoord;
out mat3 TBN;

uniform mat4 u_model;
uniform mat4 u_lightSpaceTransform;

vec4 vertexPosition;
vec3 vertexNormal;

void main()
{
	vertexPosition = (u_model * vec4(a_vertexPosition, 1.0));
	fragmentPos = vec3(vertexPosition) - u_viewPos;
	vertexNormal = normalize(mat3(transpose(inverse(u_model))) * a_vertexNormal);
	texCoord = a_texCoord;
	gl_Position = u_projection * u_view * vertexPosition;

	vec3 T = (u_model * vec4(a_tangent, 0.0)).xyz;
	T = normalize(T - dot(T, vertexNormal) * vertexNormal);
	vec3 B = cross(vertexNormal, T);
	B = normalize(B);
	TBN = mat3(T, B, vertexNormal);

}