#version 460 core
layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0, rgba16f) uniform image2D outputImg;
//layout(binding = 1) uniform vec3[]

struct Table {
	float LocalDistance;
	float Tvalue;
};

layout(std430, binding = 2) buffer LookUpTable
{
	Table BezierTable[];
};

uniform vec2 mousepos;
uniform vec3 penColor;
uniform float mousedown;
uniform float brushMode;
uniform float reset;
uniform float dt;
uniform float blur;
uniform float Progress;

uniform float size;

vec3 CubicBezier3D(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t);
vec3 QuadraticBezier3D(vec3 p0, vec3 p1, vec3 p2, float t);
vec2 rotate2D(vec2 a, float t);

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);	
	vec4 pixelCol = imageLoad(outputImg, pixel_coords) ;

	vec3 pixelPosinSpace = vec3(pixel_coords.x / 4096.0, 0, pixel_coords.y / 4096.0);
	pixelPosinSpace *= 4.0;

	vec3 startingPoint = vec3(0.5);
	vec3 endPoint = (penColor - pixelPosinSpace);
	float yOffset = endPoint.y - 0.5;
	vec3 point1 = vec3(startingPoint.x, max(2.0,(0.5 + (yOffset*2))), startingPoint.z);
	vec3 point2 = vec3(endPoint.x, max(2.0,0.5 - (yOffset * 2)), endPoint.z);;

	float distance = distance(mousepos, vec2(pixel_coords.x, pixel_coords.y));
	distance /= size;
	distance = clamp(distance, 0, 1);

	float T = 1 - distance;
	T *= Progress;
	float totalArcLength = 0;
	float targetArcLength = 0;

	for (int i = 0; i < 128; i++) {
		totalArcLength += BezierTable[i].LocalDistance;
	}
	
	targetArcLength = totalArcLength * T;

	float currentArcLength = 0;
	float prevArcLength = 0;


	int TableLocation;

	for (int i = 0; i < 128; i++) {
		currentArcLength += BezierTable[i].LocalDistance;
		if (currentArcLength > targetArcLength) {
			TableLocation = i;
			prevArcLength = currentArcLength - BezierTable[i].LocalDistance;
			break;
		}
	}

	float R = (targetArcLength - prevArcLength) / (currentArcLength - prevArcLength);

	T = mix(BezierTable[TableLocation - 1].Tvalue, BezierTable[TableLocation].Tvalue, R);
	float T2 = mix(BezierTable[TableLocation].Tvalue, BezierTable[TableLocation + 1].Tvalue, R);

	vec3 Normal = normalize(QuadraticBezier3D(startingPoint, point1, endPoint, T2) - QuadraticBezier3D(startingPoint, point1, endPoint, T));
	vec3 Tangent = normalize(cross(Normal,vec3(0,1,0)));
	vec3 BiTangent = -normalize(cross(Tangent, Normal));

	vec2 Direction = vec2(pixel_coords.x, pixel_coords.y) - mousepos;
	Direction = normalize(Direction);

	vec2 TexNormal = normalize(vec2(Normal.x, Normal.z));

	float Texdot = dot(vec2(0, 1), TexNormal);
	float Texdet = -TexNormal.x;
	float angle = atan(Texdet, Texdot);

	Direction = rotate2D(Direction, -angle);

	if (distance < 1) {
		//pixelCol.xyz = mix(startingPoint, endPoint,  pow(T,pow(T,4 * T))); //funky
		//pixelCol.xyz = mix(startingPoint, endPoint,  T); //Linear
		//pixelCol.xyz = CubicBezier3D(startingPoint, point1, point2, endPoint,  pow(T,2));//Cubic bezier
		pixelCol.xyz = CubicBezier3D(startingPoint, point1, point2, endPoint,T) + (((BiTangent * Direction.x) + (Tangent * Direction.y)) * ((T) * 0.05));//Quadratic bezier
		//pixelCol.xyz = QuadraticBezier3D(startingPoint, point1, endPoint,  pow(T,pow(T,4 * T)));//Quadratic bezier, Spikey
		//pixelCol.xyz = QuadraticBezier3D(startingPoint, point1, endPoint,  pow(T, 20));//Quadratic bezier
	}


	imageStore(outputImg, pixel_coords, pixelCol);
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

vec2 rotate2D(vec2 a, float t) {
	float c = cos(t);
	float s = sin(t);
	return vec2((c * a.x) - (s * a.y), (s * a.x) + (c * a.y));
}