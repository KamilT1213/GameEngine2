#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(std140, binding = 4) uniform b_shadowCamera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};


in float vt_Scale[];
out vec2 texCoord;
out vec3 FragPos;
out vec3 Normal;

void main()
{
	mat4 VP = u_projection * u_view;
	vec3 pos = gl_in[0].gl_Position.xyz;
	FragPos = pos - u_viewPos;
	vec3 toCamera = normalize(u_viewPos - pos);
	Normal = toCamera;

	float steep = max(abs(gl_in[0].gl_Position.y + 4),0.3);

	vec3 up = vec3(0, 1, 0);
	vec3 right = normalize(cross(toCamera, up)) * (vt_Scale[0] * 0.3);// * steep);
	
	//Bottom-left corner (-right, -up)
	gl_Position = VP * vec4(pos - right - up, 1.0);
	texCoord = vec2(0, 0);
	EmitVertex();

	//Top-left corner (-right, +up)
	gl_Position = VP * vec4(pos - right + (up * (vt_Scale[0] )), 1.0);
	texCoord = vec2(0, 1.0);
	EmitVertex();

	//Bottom-right corner (+right, -up)
	gl_Position = VP * vec4(pos + right - up, 1.0);
	texCoord = vec2(1.0, 0);
	EmitVertex();

	//Top-right corner (+right, +up)
	gl_Position = VP * vec4(pos + right + (up * (vt_Scale[0] )), 1.0);
	texCoord = vec2(1.0, 1.0);
	EmitVertex();

}