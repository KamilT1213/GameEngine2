#version 460 core
layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0, rgba16f) uniform image2D waterImg;

uniform float time;
uniform float rippleTime;
uniform float waterEaseIn;
uniform float waterEaseOut;
uniform vec2 position;
//uniform vec3

vec2 rotate2D(vec2 a, float t);
float remap(float currValue, float  inMin, float inMax, float outMin, float outMax);
vec2 hash2(vec2 p);
float noise(in vec2 p);
float ripple(float factor);

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 pixelCol = vec4(vec3(0.5), 1.0);// imageLoad(waterImg, pixel_coords);

	float result = noise((vec2(pixel_coords.x, pixel_coords.y) + vec2(time * 5)) / 50.0) * 1.0;
	// result = noise((vec2(pixel_coords.x, pixel_coords.y) + rotate2D(vec2(time * 5), time/10)) / 10.0) * 1.0;
	//result += noise((vec2(pixel_coords.x, pixel_coords.y) + vec2(time * 10)) / 5.0) * 1.0;
	pixelCol.y = remap(result, -1, 1, 0.490, 0.510);
	pixelCol.y += max(ripple(distance(position * 1024, pixel_coords)), 0);

	imageStore(waterImg, pixel_coords, pixelCol);
}

vec2 rotate2D(vec2 a, float t) {
	float c = cos(t);
	float s = sin(t);
	return vec2((c * a.x) - (s * a.y), (s * a.x) + (c * a.y));
}

float remap(float currValue, float  inMin, float inMax, float outMin, float outMax) {

	float t = (currValue - inMin) / (inMax - inMin);
	return mix(outMin, outMax, t);
}

vec2 hash2(vec2 p)
{
	p = vec2(dot(p, vec2(127.1, 311.7)),
		dot(p, vec2(269.5, 183.3)));

	return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(in vec2 p)
{
	vec2 i = floor(p);
	vec2 f = fract(p);

	vec2 u = f * f * (3.0 - 2.0 * f);

	return mix(mix(dot(hash2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
		dot(hash2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
		mix(dot(hash2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
			dot(hash2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}

float ripple(float factor) {
	float x = abs(factor);
	float y = x * 0.5;
	y += (-rippleTime) * 3.14159;
	y = sin(y) / 200;
	y += waterEaseIn;
	y += -(pow((x / 200), 2));

	float u = min(pow(x / 200, 2) - waterEaseOut, 0);

	return y + u;
}