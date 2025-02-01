#version 460 core
layout(location = 0) out vec3 gFragPos;
layout(location = 1) out vec3 gID;
layout(location = 2) out vec3 gNorm;
layout(location = 3) out vec2 gUV;

in vec3 fragmentPos;
in vec2 texCoord;
in mat3 TBN;
uniform vec3 ID;
uniform sampler2D u_normalMap;

//global vars
vec3 normal ;


void main()
{

	gFragPos = fragmentPos;
	gID = ID;
	gUV = texCoord;
	vec3 N = texture(u_normalMap, texCoord).rgb;
	normal = normalize(TBN * (N * 2.0 - 1.0));
	gNorm = normal;

}
