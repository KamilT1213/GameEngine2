#version 460 core
			
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in float a_Scale;

uniform mat4 u_model;
uniform sampler2D u_heightMap;
out float vt_Scale;

void main()
{
	

	float height = texture(u_heightMap,(a_Pos.xz/100)).r - 0.5;
	vec4 vertexPrePos = u_model * vec4(a_Pos , 1.0);
	gl_Position = vertexPrePos + vec4(0, height * 20, 0,0);
	vt_Scale = a_Scale;
}