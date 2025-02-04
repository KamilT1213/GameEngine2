#version 460 core
layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0, rgba16f) uniform image2D GroundImg;

uniform float Reset;
uniform float ResetWave;
uniform vec2 MousePos;
uniform float action;
uniform float dt;

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 sampled = imageLoad(GroundImg, pixel_coords);

	if (Reset > 0) {
		sampled.x = clamp(sampled.x, 0, ResetWave);
		sampled.z = 1;
		sampled.a = 1;
		if (ResetWave <= 0) {
			sampled.x = 1;
		}
	}

	if (action > 0) {
		ivec2 texCoords = ivec2(4096 * MousePos.x, 4096 * MousePos.y);
		float Size = (4096.0 / 20.0);
		float dist = distance(pixel_coords, texCoords) / Size;
		if (dist < 1) {
			sampled.x -= exp(-pow(dist, 2)) * dt;
			sampled.x = clamp(sampled.x, 0, 1);
		}

	}

	imageStore(GroundImg, pixel_coords, sampled);
}
