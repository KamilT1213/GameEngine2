#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tes_fragmentPos[];
in vec2 tes_texCoord[];
//in vec3 vs_vertexNormal[];

struct Particle {
	vec4 position;
	vec2 UV;
};

layout(std430, binding = 0) buffer vertex
{
	Particle p[];
};

layout(std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

uniform float editing;

out vec3 fragmentPos;
out vec2 texCoord;
out vec3 vertexNormal;
out mat3 TBN;

vec3 getNormal();

void main()
{
	

	vec2 deltaUV1 = tes_texCoord[1] - tes_texCoord[0];
	vec2 deltaUV2 = tes_texCoord[2] - tes_texCoord[0];

	vec3 deltaPos1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 deltaPos2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	float r = 1.0 / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

	vec3 T = ((deltaPos1 * deltaUV2.y) - (deltaPos2 * deltaUV1.y)) * r;
	vec3 B = ((deltaPos2 * deltaUV1.x) - (deltaPos1 * deltaUV2.x)) * r;

	TBN = mat3(normalize(T), normalize(B), getNormal());
	//T = (vs_model[0] * vec4(T, 0.0)).xyz;	
	//T = normalize(T - dot(T, getNormal()) * getNormal());
	//vec3 B = cross(getNormal(), T);
	//B = (vs_model[0] * vec4(B, 0.0)).xyz;
	

	for (int i = 0; i < 3; i++) {

		
		texCoord = tes_texCoord[i];
		fragmentPos = tes_fragmentPos[i];// +(vec3(0, 6, 0) * height * 0.5);

		//gl_Position = gl_in[i].gl_Position;// +vec4(vec3(0, 6, 0) * height * 1.0, 0);
		gl_Position = u_projection * u_view * gl_in[i].gl_Position;

		//fragmentPos = (gl_in[i].gl_Position + vec4(vec3(0, 1, 0) * height * 5.0, 0)).xyz
		vertexNormal = getNormal();

		EmitVertex();
	}
	if (dot(getNormal(), vec3(0, 1, 0)) < -0.5 && editing > 0) {

		int a = gl_PrimitiveIDIn;
		float b = a / 16384.0;
		b = mod(b, 1);
		a = int(b * 16384);

		p[a].position.xyz = gl_in[0].gl_Position.xyz;
		p[a].UV = tes_texCoord[0];

	}
	//else {
	//	p[gl_PrimitiveIDIn].position.xyz = p[gl_PrimitiveIDIn].position.xyz;
	//}

	EndPrimitive();

}

vec3 getNormal() {

	vec3 dir1 = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	vec3 dir2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	return normalize(cross(dir2, dir1));
}