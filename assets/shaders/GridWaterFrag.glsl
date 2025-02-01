#version 460 core

layout(location = 0) out vec4 colour;
layout(location = 1) out vec3 gFragPos;
layout(location = 2) out vec3 gNorm;
layout(location = 3) out vec4 gSpecMetallicRoughness;
layout(location = 4) out vec2 gTexCoord;

in vec3 fragmentPos;
in vec2 texCoord;
in vec3 vertexNormal;
in mat3 TBN;

struct directionalLight
{
	vec3 colour;
	vec3 direction;
};

struct pointLight
{
	vec3 colour;
	vec3 position;
	vec3 constants;
};

struct spotLight
{
	vec3 colour;
	vec3 position;
	vec3 direction;
	vec3 constants;
	float cutOff;
	float outerCutOff;
};

const int numPointLights = 10;
const int numSpotLights = 1;

layout(std140, binding = 1) uniform b_lights
{
	uniform directionalLight dLight;
	uniform pointLight pLights[numPointLights];
	uniform spotLight sLights[numSpotLights];
};

layout(std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};


uniform sampler2D u_albedoMap;
uniform sampler2D u_altAlbedoMap;
uniform sampler2D u_heightMap;
//uniform vec3 u_albedo;

uniform sampler2D u_specularMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_emissionMap;
uniform float u_emission;

uniform sampler2D u_depthMap;
uniform sampler2D u_shadowMap;


vec3 normal;
uniform vec3 albedoColour;
float specularStrength = 0.8;
vec3 viewDir;

vec4 tpTex(sampler2D inTex);

void main()
{
	//if (!doesPassDepthTest()) return;


	gTexCoord = texCoord;
	gFragPos = fragmentPos;
	//normal = normalize(vertexNormal);
	vec3 N =  tpTex(u_normalMap).rgb;
	normal = normalize(TBN * (N * 2.0 - 1.0));
	normal = normalize(vertexNormal);
	gNorm = normal;

	specularStrength = texture(u_specularMap, texCoord).r;
	gSpecMetallicRoughness = vec4(vec3(tpTex(u_albedoMap).r,1.0,0.3),1.0);



	float f = clamp(-pow(((texture(u_altAlbedoMap, texCoord).y - 0.5) + 0.05) * 25, 2)+0.6, 0, 1);
	colour =  mix(tpTex(u_albedoMap), vec4(1.0), f);// texture(u_albedoMap, texCoord);
}

vec4 tpTex(sampler2D inTex) {

	vec3 blendPercent = normalize(abs(normal));
	float b = (blendPercent.x + blendPercent.y + blendPercent.z);
	blendPercent = blendPercent / vec3(b);

	vec3 pos = fragmentPos.xyz + u_viewPos;

	vec4 xaxis = texture2D(inTex, pos.yz/10.0);
	vec4 yaxis = texture2D(inTex, pos.xz / 10.0);
	vec4 zaxis = texture2D(inTex, pos.xy / 10.0);

	return xaxis * blendPercent.x + yaxis * blendPercent.y + zaxis * blendPercent.z;
}