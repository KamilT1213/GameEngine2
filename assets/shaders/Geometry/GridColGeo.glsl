#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vs_fragmentPos[];
in vec2 vs_texCoord[];
//in vec3 vs_vertexNormal[];

out vec3 fragmentPos;
out vec2 texCoord;
out vec3 vertexNormal;
out mat3 TBN;

vec3 getNormal();

void main()
{

	vec2 deltaUV1 = vs_texCoord[1] - vs_texCoord[0];
	vec2 deltaUV2 = vs_texCoord[2] - vs_texCoord[0];

	vec3 deltaPos1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 deltaPos2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

	vec3 T = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
	vec3 B = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

	TBN = mat3(T, B, getNormal());
	//T = (vs_model[0] * vec4(T, 0.0)).xyz;
	//T = normalize(T - dot(T, getNormal()) * getNormal());
	//vec3 B = cross(getNormal(), T);
	//B = (vs_model[0] * vec4(B, 0.0)).xyz;
	

	for (int i = 0; i < 3; i++) {

		texCoord = vs_texCoord[i];
		fragmentPos = vs_fragmentPos[i];
		vertexNormal = getNormal();
		gl_Position = gl_in[i].gl_Position;

		EmitVertex();

	}


	EndPrimitive();

}

vec3 getNormal() {

	vec3 dir1 = vs_fragmentPos[0] - vs_fragmentPos[1];
	vec3 dir2 = vs_fragmentPos[2] - vs_fragmentPos[0];

	return normalize(cross(dir1, dir2));
}