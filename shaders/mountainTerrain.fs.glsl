#version 410
    
uniform vec3 u_color;

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

in vec4 v_normal;
in vec4 v_pos;

in float height;

out vec4 fragColor;

void main()
{
	vec3 ambient_component = u_mat_ambient.xyz;
	
	vec3 v = normalize(u_camera_pos.xyz - v_pos.xyz);
	vec3 l1 = normalize(u_light_1_pos.xyz - v_pos.xyz);
	vec3 l2 = normalize(u_light_2_pos.xyz - v_pos.xyz);
	vec3 r1 = (v_normal.xyz * 2.0 * dot(l1, v_normal.xyz)) - l1;
	vec3 r2 = (v_normal.xyz * 2.0 * dot(l2, v_normal.xyz)) - l2;
	
	vec3 mat_diffuse = u_mat_diffuse.xyz;

	vec3 diffuse_light_1 = u_light_1_diffuse.xyz * mat_diffuse.xyz * max(dot(l1, v_normal.xyz), 0.0);
	vec3 diffuse_light_2 = u_light_2_diffuse.xyz * mat_diffuse.xyz * max(dot(l2, v_normal.xyz), 0.0);
	vec3 diffuse_component = diffuse_light_1 + diffuse_light_2;
	
	vec3 specular_light_1 = u_light_1_specular.xyz * u_mat_specular.xyz * pow(max(dot(r1, v), 0.0), u_mat_shininess);
	vec3 specular_light_2 = u_light_2_specular.xyz * u_mat_specular.xyz * pow(max(dot(r2, v), 0.0), u_mat_shininess);
	vec3 specular_component = specular_light_1 + specular_light_2;
	
	fragColor = vec4(ambient_component + diffuse_component + specular_component + (abs(height) / 400.0), 1.0);
}