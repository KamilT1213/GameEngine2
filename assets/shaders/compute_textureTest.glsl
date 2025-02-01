#version 460 core
layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0, rgba16f) uniform image2D displacementImg;
//layout(binding = 1) uniform vec3[]

uniform vec2 mousepos;
uniform vec3 penColor;
uniform float mousedown;
uniform float brushMode;
uniform float reset;
uniform float dt;
uniform float blur;

uniform float size;

vec2 rotate2D(vec2 a, float t);

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);	
	vec4 pixelCol = imageLoad(displacementImg, pixel_coords);
	vec3 pixelColCentre = imageLoad(displacementImg, ivec2(mousepos.x, mousepos.y)).xyz;// -vec4(vec3(0.1) * dt, 0.0);
	vec3 Displacememt = abs(pixelColCentre - vec3(0.5));
	Displacememt *= pow(distance(vec3(0), Displacememt),2);
	if (reset > 0) {
		pixelCol.xyz = vec3(0.5001);
		pixelCol.a = 1.0;
	}
	vec3 normal = vec3(penColor.x, abs(penColor.y), penColor.z ) + vec3(0.001);
	vec3 rightTan = normalize(cross(normal, vec3(0, -1, 0)));
	vec3 upTan = normalize(cross(-rightTan, normal));
	float angle = acos(dot(vec3(0, 0, 1), rightTan));

	float disFactor =  sqrt(((1 + (Displacememt.x + Displacememt.y + Displacememt.z))));

	float scale = sqrt(disFactor)/2.0;

	vec2 transformed = rotate2D(pixel_coords - mousepos,(3.1415) - (angle * sign(dot(upTan, vec3(0, 0, 1)))));
	transformed.x = (transformed.x - (sin(acos(dot(vec3(0, 1, 0), normal))) * disFactor)) * (1 / dot(vec3(0, 1, 0), normal));
	transformed += mousepos;

	if (normal.y > 0.99) {
		transformed = pixel_coords;
	}

	float x = distance(mousepos, transformed) * scale;
	
	float strength = (-pow(abs(x / (size)), 2)) + 1;
	if (mousedown > 0) {
		pixelCol += vec4(penColor * clamp(strength, 0.0, 1.0) * dt * 2.5 * (brushMode), 1.0) ;
	}

	//rotate based on normal
	/*
	surface normal = n
	camera right = r
	tangent = t = norm(n x r)

	

	*/
	imageStore(displacementImg, pixel_coords, pixelCol);
}

vec2 rotate2D(vec2 a, float t) {
	float c = cos(t);
	float s = sin(t);
	return vec2((c * a.x) - (s * a.y), (s * a.x) + (c * a.y));
}
