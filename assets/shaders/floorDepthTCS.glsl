#version 460 core
layout(vertices = 3) out;

in vec3 vs_fragmentPos[];
in vec2 vs_texCoord[];
in vec3 vs_vertPos[];
uniform sampler2D u_heightMap;

out vec3 tcs_fragmentPos[];
out vec2 tcs_texCoord[];
out vec3 tcs_vertPos[];

layout(std140, binding = 4) uniform b_shadowCamera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

float GetTessLevel(float Distance0, float Distance1);
vec3 Displacememt = vec3(0);
void main()
{
	float tesslevel = 4.0;
	Displacememt = (texture(u_heightMap, vs_texCoord[gl_InvocationID]).rgb - vec3(0.5)) * 25.0;
	float eyeToVertexDist0 = distance(u_viewPos, vs_vertPos[0] + Displacememt);
	float eyeToVertexDist1 = distance(u_viewPos, vs_vertPos[1] + Displacememt);
	float eyeToVertexDist2 = distance(u_viewPos, vs_vertPos[2] + Displacememt);
	

	Displacememt = abs(Displacememt)/25;

	if (gl_InvocationID == 0) {
		gl_TessLevelOuter[0] = GetTessLevel(eyeToVertexDist1, eyeToVertexDist2);
		gl_TessLevelOuter[1] = GetTessLevel(eyeToVertexDist2, eyeToVertexDist0);
		gl_TessLevelOuter[2] = GetTessLevel(eyeToVertexDist0, eyeToVertexDist1);
		gl_TessLevelInner[0] = gl_TessLevelOuter[2];
	}

	tcs_fragmentPos[gl_InvocationID] = vs_fragmentPos[gl_InvocationID];
	tcs_texCoord[gl_InvocationID] = vs_texCoord[gl_InvocationID];
	tcs_vertPos[gl_InvocationID] = vs_vertPos[gl_InvocationID];

}

float GetTessLevel(float Distance0, float Distance1) {
	float AvgDistance = (Distance0 + Distance1) / 2.0;
	return (1 + (Displacememt.x + Displacememt.y + Displacememt.z)) * 2 * exp(-(AvgDistance - 15) / 15.0);
}