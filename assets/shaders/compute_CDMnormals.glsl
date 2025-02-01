#version 460 core
layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0, rgba16f) uniform image2D displacementImg;
layout(binding = 1, rgba16f) uniform image2D terrainNormalImg;
//layout(binding = 1) uniform vec3[]

vec4 Slerp(vec4 p0, vec4 p1, float t);

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	vec3 pixelPosinSpace0 = vec3((pixel_coords.x - 1) / 4096.0, 0, pixel_coords.y / 4096.0);
	pixelPosinSpace0 *= 4.0;
	pixelPosinSpace0 = (pixelPosinSpace0 - vec3(0.5)) * 25;

	vec3 pixelPosinSpace1 = vec3((pixel_coords.x + 1) / 4096.0, 0, pixel_coords.y / 4096.0);
	pixelPosinSpace1 *= 4.0;
	pixelPosinSpace1 = (pixelPosinSpace1 - vec3(0.5)) * 25;

	vec3 pixelPosinSpace2 = vec3(pixel_coords.x / 4096.0, 0, (pixel_coords.y + 1) / 4096.0);
	pixelPosinSpace2 *= 4.0;
	pixelPosinSpace2 = (pixelPosinSpace2 - vec3(0.5)) * 25;

	vec3 pixelPosinSpace3 = vec3(pixel_coords.x / 4096.0, 0, (pixel_coords.y - 1) / 4096.0);
	pixelPosinSpace3 *= 4.0;
	pixelPosinSpace3 = (pixelPosinSpace3 - vec3(0.5)) * 25;

	vec3 displacement0 = (imageLoad(displacementImg, pixel_coords + ivec2(-1, 0)).xyz - vec3(0.5)) * 25;
	vec3 displacement1 = (imageLoad(displacementImg, pixel_coords + ivec2(1, 0)).xyz - vec3(0.5)) * 25;
	vec3 displacement2 = (imageLoad(displacementImg, pixel_coords + ivec2(0, 1)).xyz - vec3(0.5)) * 25;
	vec3 displacement3 = (imageLoad(displacementImg, pixel_coords + ivec2(0, -1)).xyz - vec3(0.5)) * 25;

	pixelPosinSpace0 += displacement0;
	pixelPosinSpace1 += displacement1;
	pixelPosinSpace2 += displacement2;
	pixelPosinSpace3 += displacement3;

	vec3 dir1 = pixelPosinSpace0 - pixelPosinSpace1;
	vec3 dir2 = pixelPosinSpace2 - pixelPosinSpace3;
	vec3 norm = normalize(cross(dir1, dir2));

	//

	vec3 pixelPosinSpace4 = vec3((pixel_coords.x - 1) / 4096.0, 0, (pixel_coords.y - 1) / 4096.0);
	pixelPosinSpace4 *= 4.0;
	pixelPosinSpace4 = (pixelPosinSpace4 - vec3(0.5)) * 25;

	vec3 pixelPosinSpace5 = vec3((pixel_coords.x + 1) / 4096.0, 0, (pixel_coords.y + 1) / 4096.0);
	pixelPosinSpace5 *= 4.0;
	pixelPosinSpace5 = (pixelPosinSpace5 - vec3(0.5)) * 25;

	vec3 pixelPosinSpace6 = vec3((pixel_coords.x - 1) / 4096.0, 0, (pixel_coords.y + 1) / 4096.0);
	pixelPosinSpace6 *= 4.0;
	pixelPosinSpace6 = (pixelPosinSpace6 - vec3(0.5)) * 25;

	vec3 pixelPosinSpace7 = vec3((pixel_coords.x + 1) / 4096.0, 0, (pixel_coords.y - 1) / 4096.0);
	pixelPosinSpace7 *= 4.0;
	pixelPosinSpace7 = (pixelPosinSpace7 - vec3(0.5)) * 25;

	vec3 displacement4 = (imageLoad(displacementImg, pixel_coords + ivec2(-1, -1)).xyz - vec3(0.5)) * 25;
	vec3 displacement5 = (imageLoad(displacementImg, pixel_coords + ivec2(1, 1)).xyz - vec3(0.5)) * 25;
	vec3 displacement6 = (imageLoad(displacementImg, pixel_coords + ivec2(-1, 1)).xyz - vec3(0.5)) * 25;
	vec3 displacement7 = (imageLoad(displacementImg, pixel_coords + ivec2(1, -1)).xyz - vec3(0.5)) * 25;

	pixelPosinSpace4 += displacement4;
	pixelPosinSpace5 += displacement5;
	pixelPosinSpace6 += displacement6;
	pixelPosinSpace7 += displacement7;

	vec3 dir3 = pixelPosinSpace4 - pixelPosinSpace5;
	vec3 dir4 = pixelPosinSpace6 - pixelPosinSpace7;
	vec3 norm2 = normalize(cross(dir3, dir4));


	vec4 Col = Slerp(vec4(norm, 1.0), vec4(norm2, 1.0), 0.5);
	//Col = vec4(norm, 1.0);
	if (isnan(Col).x) Col = imageLoad(terrainNormalImg, pixel_coords + ivec2(1,0));
	//Col = vec4(vec2(pixel_coords.x/ 4096.0,pixel_coords.y / 4096.0),0,1);
	imageStore(terrainNormalImg, pixel_coords, Col);
}

vec4 Slerp(vec4 p0, vec4 p1, float t)
{
	float dotp = dot(normalize(p0), normalize(p1));
	if ((dotp > 0.9999) || (dotp < -0.9999))
	{
		if (t <= 0.5)
			return p0;
		return p1;
	}
	float theta = acos(clamp(dotp, -1, 1));
	if (theta == 0) theta = 0.001;
	vec4 P = ((p0 * sin((1 - t) * theta) + p1 * sin(t * theta)) / sin(theta));
	P.w = 1;
	return P;
}