#version 410

uniform vec4 u_camera_pos;
uniform vec4 u_mat_ambient;
uniform vec4 u_mat_diffuse;
uniform vec4 u_mat_specular;
uniform float u_mat_shininess;

uniform vec4 u_light_1_pos;
uniform vec4 u_light_1_ambient;
uniform vec4 u_light_1_diffuse;
uniform vec4 u_light_1_specular;

uniform vec4 u_light_2_pos;
uniform vec4 u_light_2_ambient;
uniform vec4 u_light_2_diffuse;
uniform vec4 u_light_2_specular;

uniform bool u_use_phong;
uniform bool u_use_texture_map;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

uniform mat4 u_transform;
uniform mat4 u_viewProjection;

out vec4 v_normal;
out vec4 v_pos;

uniform vec2 u_offset;
out float height;

float NOISE_GRANULARITY = 0.02;
int   OCTAVES = 8;

// Random number in range [0,1)
float random(vec2 st)
{
	return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// 2D signed value noise in range [0,1)
float signedValueNoise(vec2 st)
{
	vec2 gridPos   = floor(st);
	vec2 squarePos = fract(st);

	float botL = random(gridPos);
	float botR = random(gridPos + vec2(1.0, 0.0));
	float topL = random(gridPos + vec2(0.0, 1.0));
	float topR = random(gridPos + vec2(1.0, 1.0));

	float botVal = mix(botL, botR, smoothstep(0.0, 1.0, squarePos.x));
	float topVal = mix(topL, topR, smoothstep(0.0, 1.0, squarePos.x));
	float value  = mix(botVal, topVal, smoothstep(0.0, 1.0, squarePos.y));

	return 2.0 * value - 1.0;
}

// Create ridges using octaves of signed value noise
float ridge(vec2 st)
{
	float value = 0.0;
	float frequency = 1.0;
	float amplitude = 20.0;
	float lancularity = 1.5;
	float gain = 0.6;

	for (int i = 0; i < OCTAVES; i++)
	{
		value += amplitude * abs(signedValueNoise(frequency * st));
		frequency *= lancularity;
		amplitude *= gain;
	}

	float ridges = 1.0 - value;
	return ridges;
}

void main()
{
	int offsetX = (gl_InstanceID / 100) - 50;
	int offsetY = (gl_InstanceID % 100) - 50;

	vec2 st = vec2(a_position.x + 2.0 * offsetX, a_position.z + 2.0 * offsetY);
	st *= NOISE_GRANULARITY;
	vec3 heightMappedPos = a_position;
	heightMappedPos.y += ridge(st);

	height = heightMappedPos.y;

	heightMappedPos *= 10.0f;
	heightMappedPos.x += 20.0f * offsetX;
	heightMappedPos.z += 20.0f * offsetY;
	heightMappedPos.y -= 10.0f;

	v_normal = normalize(u_transform * vec4(a_normal, 0.0));
	v_pos = vec4(heightMappedPos, 1.0);

	gl_Position = u_viewProjection * v_pos;
}
