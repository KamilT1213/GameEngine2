#version 460 core

layout(location = 0) out vec4 Outcolour;

//precision highp sampler2D;

in vec2 texCoords;

uniform sampler2D u_gColour;
uniform sampler2D u_gFragPosition;
uniform sampler2D u_gNormal;
uniform sampler2D u_gSpecMetallicRoughness;
uniform sampler2D u_gDepth;

uniform mat4 u_lightSpaceMatrix;
uniform sampler2D u_lightDepthMap;


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



bool doesPassDepthTest();
vec3 getDirectionalLight();
float ShadowCalculation();
vec3 getPointLight(int idx);
vec3 getSpotLight(int idx);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(float NdotH);
float GeometrySmith(float NdotV, float NdotL);
float GeometrySchlickGGC(float Ndot);


//global vars
vec3 normal;
vec3 albedoColour;
float specularStrength = 0.8;
vec3 viewDir;
vec4 fragPosClipSpace;
vec3 fragmentPos;
vec4 fragmentPosLightSpace;
uniform vec2 u_ScreenSize;
float ambientStrength = 0.05;
float shadow = 0.0;
float metallic = 0.0;
float roughness = 0.0;
uniform float u_Toggle;

vec3 F0;

void main()
{
	fragPosClipSpace = u_projection * u_view * texture(u_gFragPosition, texCoords);


	//if (!doesPassDepthTest()) { Outcolour = texture(u_gColour, texCoords); return; }



	vec3 result = vec3(0.0, 0.0, 0.0);

	vec4 sampled = texture(u_gDepth, texCoords);
	
	

	fragmentPos = texture(u_gFragPosition, texCoords).rgb + u_viewPos;

	albedoColour = texture(u_gColour, texCoords).xyz;
	viewDir = normalize(u_viewPos - (fragmentPos));
	normal = texture(u_gNormal, texCoords).rgb;
	specularStrength = texture(u_gSpecMetallicRoughness, texCoords).r;
	metallic = texture(u_gSpecMetallicRoughness, texCoords).g;
	roughness = texture(u_gSpecMetallicRoughness, texCoords).b;


	F0 = mix(vec3(0.04), albedoColour, metallic);


	//albedoColour = texture(u_albedoMap, texCoord).rgb;

	// light casters
	shadow = ShadowCalculation();
	
	if ( sampled.x > 0.999) {
		result = texture(u_gColour, texCoords).rgb;
	}
	else
	{
		result += getDirectionalLight()/3.0;

		for (int i = 0; i < numPointLights; i++)
		{
			result += getPointLight(i);
		}

		for (int i = 0; i < numSpotLights; i++)
		{
			//result += getSpotLight(i);
		}
	}

		 
	Outcolour = ((vec4(result, 1.0) * vec4(albedoColour,1.0)) );
}

bool doesPassDepthTest()
{
	float fragmentDepth = fragPosClipSpace.z / fragPosClipSpace.w;
	fragmentDepth = fragmentDepth * 0.5 + 0.5;

	vec2 fragCoord = (fragPosClipSpace.xy / fragPosClipSpace.w) * 0.5 + 0.5;
	float depthPrepass = texture(u_gDepth, texCoords).r;

	float bias = 0.01;
	if (fragmentDepth >= depthPrepass + bias) return false;
	if (fragmentDepth > 0.999) return false;
	return true;
}

float ShadowCalculation() {

	float shadow = 0.0;
	float pixelSize = 1.0 / (1024.0 *8);

	fragmentPosLightSpace = u_lightSpaceMatrix * vec4(fragmentPos,1.0);//texture(u_gLSMap, texCoords);
	vec3 projCoords = fragmentPosLightSpace.xyz / fragmentPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	float currentDepth = projCoords.z;

	float closestDepth = 0;

	vec2 clampedProjCoords = projCoords.xy -mod(projCoords.xy, pixelSize);


	vec2 SubCoords = (mod(projCoords.xy + vec2(-0.5 * pixelSize), pixelSize) - vec2(0.5 * pixelSize)) * (1024.0 * 2 *8);

	float xOffset = (sign(SubCoords.x) - 1) * 0.5;
	float yOffset = (sign(SubCoords.y) - 1) * 0.5;

	int flag = 0;

	int counter = 8;

	float bias = 0.004;
	closestDepth = texture(u_lightDepthMap, clampedProjCoords).r;
	if (currentDepth >= closestDepth + bias) shadow = 0.5;
	return shadow;

	for (float x = 0 - xOffset; x <= 1 - xOffset; x++) {
		for (float y = 0 - yOffset; y <= 1 - yOffset; y++) {
			closestDepth = texture(u_lightDepthMap, clampedProjCoords + (vec2(x,y) * pixelSize) - vec2(pixelSize)).r;
			if (currentDepth >= closestDepth + bias) flag += counter;
			counter /= 2;
		}
	}

	/*
	8 | 2
	_____
	4 | 1

	*/


	float Calc[16];

	float x = SubCoords.x;
	float y = SubCoords.y;
	
									 
	Calc[0] = 0;                     // |  |
	Calc[1] = sign(x + y - 1);       // | .|
	Calc[2] = sign(x - y - 1);       // | `|
	Calc[3] = sign(x);               // | :|
	Calc[4] = sign(y - x - 1);       // |. |
	Calc[5] = sign(-y) * -1;         // |..|
	Calc[6] = 0;                     // |.`|
	Calc[7] = sign(x - -y + 1);      // |.:|
	Calc[8] = sign(x + y + 1) * - 1; // |` |
	Calc[9] = 0;                     // |`.|
	Calc[10] = sign(-y);             // |``|
	Calc[11] = sign(-y + x + 1);     // |`:|
	Calc[12] = sign(x) * -1;         // |: |
	Calc[13] = sign(y - x + 1);      // |:.|
	Calc[14] = sign(-y - x + 1);     // |:`|
	Calc[15] = 1;                    // |::|

	//if (currentDepth >= closestDepth + bias) shadow = 0.5;
	//closestDepth = texture(u_lightDepthMap, projCoords.xy).r;
	if (currentDepth >= closestDepth + bias) shadow += 0.5 * u_Toggle;
	if (Calc[flag] > 0) shadow = 0.5 * (1 -u_Toggle);
	return shadow;

}


vec3 getDirectionalLight()
{
	vec3 V = viewDir;
	vec3 L = normalize(-dLight.direction);
	vec3 H = normalize(L + V);
	vec3 N = normal;

	float NdotL = max(dot(N, L), 0.0001);
	float NdotH = max(dot(N, H), 0.0001);
	float NdotV = max(dot(N, V), 0.0001);

	vec3 F = fresnelSchlick(NdotH, F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	
	//return diffuse;

	float D = DistributionGGX(NdotH);
	float G = GeometrySmith(NdotV, NdotL);


	vec3 numerator = D * G * F;
	float denominator = 4.0 * NdotV * NdotL + 0.0001;
	vec3 specular = numerator / denominator;


	vec3 ambient = ambientStrength * vec3(1.0) * albedoColour;

	vec3 diffuse = (kD * albedoColour / 3.1415 ) * NdotL;

	return ambient + (1.0 - shadow) + (diffuse + specular) * (dLight.colour);

	//float diffuseFactor = max(dot(normal, -dLight.direction), 0.0);
	//vec3 diffuse = diffuseFactor * dLight.colour;

	//vec3 H = normalize(-dLight.direction + viewDir);
	//float specularFactor = pow(max(dot(normal, H), 0.0), 64);
	//vec3 specular = dLight.colour * specularFactor * specularStrength;

	//return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 getPointLight(int idx)
{

	vec3 V = viewDir;
	vec3 L = normalize((pLights[idx].position) - fragmentPos);
	vec3 H = normalize(L + V);
	vec3 N = normal;

	float distance = length((pLights[idx].position ) - fragmentPos) / pLights[idx].constants.x;
	float attn = 1.0 / (distance * distance);//(pLights[idx].constants.x + (pLights[idx].constants.y * distance) + (pLights[idx].constants.z * (distance * distance)));

	float NdotL = max(dot(N, L), 0.0001);
	float NdotH = max(dot(N, H), 0.0001);
	float NdotV = max(dot(N, V), 0.0001);

	vec3 F = fresnelSchlick(NdotH, F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;


	//return diffuse;

	float D = DistributionGGX(NdotH);
	float G = GeometrySmith(NdotV, NdotL);


	vec3 numerator = D * G * F;
	float denominator = 4.0 * NdotV * NdotL + 0.0001;
	vec3 specular = numerator / denominator;


	vec3 ambient = ambientStrength * vec3(1.0) * albedoColour;

	vec3 diffuse = (kD * albedoColour / 3.1415 ) * NdotL;

	return ambient + (diffuse + specular) * (pLights[idx].colour * attn);

	/*vec3 ambient = ambientStrength * pLights[idx].colour;

	float distance = length(pLights[idx].position - fragmentPos);
	float attn = 1.0 / (pLights[idx].constants.x + (pLights[idx].constants.y * distance) + (pLights[idx].constants.z * (distance * distance)));
	vec3 lightDir = normalize(pLights[idx].position - fragmentPos);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * attn * pLights[idx].colour;

	vec3 H = normalize(lightDir + viewDir);
	float specularFactor = pow(max(dot(normal, H), 0.0), 64);
	vec3 specular = pLights[idx].colour * specularFactor * specularStrength;

	return ambient + (1.0 - shadow) * (diffuse + specular);*/
}

vec3 getSpotLight(int idx)
{
	vec3 lightDir = normalize(sLights[idx].position - fragmentPos);
	float theta = dot(lightDir, normalize(-sLights[idx].direction));
	//float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * sLights[idx].colour;

	if (theta > sLights[idx].outerCutOff)
	{

		float distance = length(sLights[idx].position - fragmentPos);
		float attn = 1.0 / (sLights[idx].constants.x + (sLights[idx].constants.y * distance) + (sLights[idx].constants.z * (distance * distance)));
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * attn * sLights[idx].colour;

		float specularStrength = 0.8;

		vec3 H = normalize(-lightDir + viewDir);
		float specularFactor = pow(max(dot(normal, H), 0.0), 64);
		vec3 specular = pLights[idx].colour * specularFactor * specularStrength;

		float epsilon = sLights[idx].cutOff - sLights[idx].outerCutOff;
		float intensity = clamp((theta - sLights[idx].outerCutOff) / epsilon, 0.0, 1.0);

		diffuse *= intensity;
		specular *= intensity;

		return ambient + (1.0 - shadow) * (diffuse + specular);
	}
	else
	{
		return ambient;
	}
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
};

float DistributionGGX(float NdotH) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH2 = NdotH * NdotH;

	float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
	denominator = 3.1415 * denominator * denominator;

	return a2 / denominator;
};

float GeometrySmith(float NdotV, float NdotL) {
	float ggx2 = GeometrySchlickGGC(NdotV);
	float ggx1 = GeometrySchlickGGC(NdotL);

	return ggx1 * ggx2;
};
float GeometrySchlickGGC(float Ndot) {
	float r = (roughness + 1);
	float k = (r * r) / 8.0;
	float denominator = Ndot * (1.0 - k) + k;

	return Ndot / denominator;
};
