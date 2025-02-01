#version 460 core
layout(local_size_x = 1) in;

struct Table {
	float LocalDistance;
	float Tvalue;
};

layout(std430, binding = 2) buffer LookUpTable
{
	Table BezierTable[];
};

uniform vec3 p0;
uniform vec3 p1;
uniform vec3 p2;
uniform vec3 p3;

vec3 CubicBezier3D(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t);
vec3 QuadraticBezier3D(vec3 p0, vec3 p1, vec3 p2, float t);

void main()
{
	float T = (gl_GlobalInvocationID.x) / 128.0;
	if (T != 0) {

		float T2 = (gl_GlobalInvocationID.x - 1) / 128.0;

		vec3 Pos1 = CubicBezier3D(p0, p1, p2, p3, T2);
		vec3 Pos2 = CubicBezier3D(p0, p1, p2, p3, T);

		BezierTable[gl_GlobalInvocationID.x].LocalDistance = distance(Pos1,Pos2);
		BezierTable[gl_GlobalInvocationID.x].Tvalue = T;

	}
	else {
		BezierTable[gl_GlobalInvocationID.x].LocalDistance = 0;
		BezierTable[gl_GlobalInvocationID.x].Tvalue = 0;
	}
}

vec3 CubicBezier3D(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t) {

	vec3 pp0 = mix(p0, p1, t);
	vec3 pp1 = mix(p1, p2, t);
	vec3 pp2 = mix(p2, p3, t);

	vec3 ppp0 = mix(pp0, pp1, t);
	vec3 ppp1 = mix(pp1, pp2, t);

	return mix(ppp0, ppp1, t);
}

vec3 QuadraticBezier3D(vec3 p0, vec3 p1, vec3 p2, float t) {

	vec3 pp0 = mix(p0, p1, t);
	vec3 pp1 = mix(p1, p2, t);

	return mix(pp0, pp1, t);
}