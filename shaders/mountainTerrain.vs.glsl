#version 410

uniform int u_tileDimensions;

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

// Calculate normal for vertex by testing points a half-step away
vec3 calculateNormal(vec3 position)
{
	int tileOffsetX = (gl_InstanceID / 100) - 50;
	int tileOffsetZ = (gl_InstanceID % 100) - 50;

	float testPointOffset = 1.0f / (u_tileDimensions - 1);
	mat4x3 testPoints;
	testPoints[0] = vec3(position.x - testPointOffset, 0.0, position.z);
	testPoints[1] = vec3(position.x + testPointOffset, 0.0,  position.z);
	testPoints[2] = vec3(position.x, 0.0, position.z - testPointOffset);
	testPoints[3] = vec3(position.x, 0.0, position.z + testPointOffset);
	
	for(int i = 0; i < 4; ++i)
	{
		vec2 st = vec2(testPoints[i].x + 2.0 * tileOffsetX, testPoints[i].z + 2.0 * tileOffsetZ);
		st *= NOISE_GRANULARITY;
		testPoints[i].y = ridge(st);
	}
	
	vec3 upLeftNormal = cross(testPoints[3] - position, testPoints[0] - position);
	vec3 downRightNormal = cross(testPoints[2] - position, testPoints[1] - position);
	
	return normalize(upLeftNormal + downRightNormal);
}

void main()
{
	int tileOffsetX = (gl_InstanceID / 100) - 50;
	int tileOffsetZ = (gl_InstanceID % 100) - 50;

	vec2 st = vec2(a_position.x + 2.0 * tileOffsetX, a_position.z + 2.0 * tileOffsetZ);
	st *= NOISE_GRANULARITY;
	vec3 heightMappedPosition = a_position;
	heightMappedPosition.y += ridge(st);
	vec3 normal = calculateNormal(heightMappedPosition);

	heightMappedPosition *= 10.0f;
	heightMappedPosition.x += 20.0f * tileOffsetX;
	heightMappedPosition.z += 20.0f * tileOffsetZ;
	heightMappedPosition.y -= 10.0f;

	v_normal = normalize(u_transform * vec4(normal, 0.0));
	v_pos = vec4(heightMappedPosition, 1.0);

	gl_Position = u_viewProjection * v_pos;
}
