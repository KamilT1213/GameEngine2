#version 460 core

layout(location = 0) out vec4 colour;
layout(location = 1) out vec3 gFragPos;
layout(location = 2) out vec3 gNorm;
layout(location = 3) out vec4 gSpec;

in vec3 texCoords;

uniform samplerCube u_skybox;

void main() 
{
	//vec4(texCoords,0);//
	colour =  texture(u_skybox, texCoords);

	gFragPos = vec3(0);

	gNorm = vec3(0);

	gSpec = vec4(0);
}