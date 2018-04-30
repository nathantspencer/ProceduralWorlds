#include "Application.h"
#include "Terrain/MountainTerrainTile.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

GLuint CompileShader(const char* src, GLint type)
{
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &src, NULL);

	glCompileShader(shader);
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	GLint infoLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

	if (infoLen > 1)
	{
		printf("%s during shader compilation.\n ", compiled == GL_TRUE ? "Warning" : "Error");
		char* buf = new char[infoLen];
		glGetShaderInfoLog(shader, infoLen, NULL, buf);
		printf("Compilation log: %s\n", buf);
		delete[] buf;
	}
	
	return shader;
}

Application::Application()
{
	gl3wInit();

	const char* OpenGLversion = (const char*) glGetString(GL_VERSION);
	const char* GLSLversion = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("OpenGL %s\nGLSL: %s\n\n", OpenGLversion, GLSLversion);

	glClearColor(0.30f, 0.45f, 0.68f, 1.00f);
	glClearDepth(1.0f);

	const char* vertex_shader_src = R"(
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
	)";

	const char* fragment_shader_src = R"(
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
	)";

	int vertex_shader_handle = CompileShader(vertex_shader_src, GL_VERTEX_SHADER);
	int fragment_shader_handle = CompileShader(fragment_shader_src, GL_FRAGMENT_SHADER);

	m_program = glCreateProgram();

	glAttachShader(m_program, vertex_shader_handle);
	glAttachShader(m_program, fragment_shader_handle);

	glLinkProgram(m_program);

	int linked;
	glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char* buf = new char[infoLen];
			glGetProgramInfoLog(m_program, infoLen, NULL, buf);
			printf("Linking error: \n%s\n", buf);
			delete[] buf;
		}
	}

	glDetachShader(m_program, vertex_shader_handle);
	glDetachShader(m_program, fragment_shader_handle);

	glDeleteShader(vertex_shader_handle);
	glDeleteShader(fragment_shader_handle);

	m_uniform_transform = glGetUniformLocation(m_program, "u_transform");
	m_uniform_viewProjection = glGetUniformLocation(m_program, "u_viewProjection");
	
	m_uniform_color = glGetUniformLocation(m_program, "u_color");
	m_uniform_ambient = glGetUniformLocation(m_program, "u_ambient");
    m_uniform_light_pos = glGetUniformLocation(m_program, "u_light_pos");
    
    m_uniform_offset = glGetUniformLocation(m_program, "u_offset");
    
    m_mat_ambient = glGetUniformLocation(m_program, "u_mat_ambient");
    m_mat_diffuse = glGetUniformLocation(m_program, "u_mat_diffuse");
    m_mat_specular = glGetUniformLocation(m_program, "u_mat_specular");
    m_mat_shininess = glGetUniformLocation(m_program, "u_mat_shininess");
    
    m_light_1_pos = glGetUniformLocation(m_program, "u_light_1_pos");
    m_light_1_ambient = glGetUniformLocation(m_program, "u_light_1_ambient");
    m_light_1_diffuse = glGetUniformLocation(m_program, "u_light_1_diffuse");
    m_light_1_specular = glGetUniformLocation(m_program, "u_light_1_specular");
    
    m_light_2_pos = glGetUniformLocation(m_program, "u_light_2_pos");
    m_light_2_ambient = glGetUniformLocation(m_program, "u_light_2_ambient");
    m_light_2_diffuse = glGetUniformLocation(m_program, "u_light_2_diffuse");
    m_light_2_specular = glGetUniformLocation(m_program, "u_light_2_specular");
    
    m_use_phong = glGetUniformLocation(m_program, "u_use_phong");
    m_use_texture_map = glGetUniformLocation(m_program, "u_use_texture_map");
    m_camera_pos = glGetUniformLocation(m_program, "u_camera_pos");

	m_teapot.Load("../../terrainTile.obj");
    
    m_cameraPosition = glm::vec3(0.0f, 0.0f, -50.0f);
    m_cameraAzimuth = 0.0f;
    m_cameraAltitude = 0.0f;
    
    m_sensitivity = 40.0f;
    m_lastTime = 0.0f;
    m_shadingModel = 1;
    
    m_isKeyPressed['W'] = false;
    m_isKeyPressed['A'] = false;
    m_isKeyPressed['S'] = false;
    m_isKeyPressed['D'] = false;
    
    m_isMouseButtonPressed = false;
    
    m_light2Angle = 0.0f;
    m_light2Height = 50.0f;
    m_light2Radius = 50.0f;
}

void Application::KeyCallback(int keycode, int event)
{
    char character = char(keycode);
    m_isKeyPressed[character] = bool(event);
}

void Application::MouseButtonCallback(int buttoncode, int event)
{
    if (buttoncode == GLFW_MOUSE_BUTTON_1)
    {
        m_isMouseButtonPressed = bool(event);
    }
}

void Application::CursorPosCallback(float x, float y)
{
    m_currentMousePosition = glm::vec2(x, y);
}

void Application::ScrollCallback(float x, float y)
{
}

void Application::SetLight2Radius(float radius)
{
    m_light2Radius = radius;
}

void Application::SetLight2Angle(float angle)
{
    m_light2Angle = angle;
}

void Application::SetLight2Height(float height)
{
    m_light2Height = height;
}

Application::~Application()
{
	glDeleteProgram(m_program);
}

inline void* ToVoidPointer(int offset)
{
	size_t offset_ = static_cast<size_t>(offset);
	return reinterpret_cast<void*>(offset_);
}

void Application::Draw(float time, float aspect)
{
    float deltaTime = time - m_lastTime;
    int fps = (int) (1.0f / deltaTime);
    fprintf(stderr, "\nFPS: %d", (int)fps);
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glCullFace(GL_BACK);
    
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glUseProgram(m_program);

	glm::mat4 transform = glm::mat4(1.0);
	glUniformMatrix4fv(m_uniform_transform, 1, GL_FALSE, &transform[0][0]);

	glm::mat4 projection = glm::perspective(45.0f, aspect, 5.0f, 10000.0f);
    glm::mat4 view = glm::mat4();
    
    // Determine azimuth and altitude angles
    float deltaAzimuth;
    float deltaAltitude;
    
    if (m_isMouseButtonPressed)
    {
        float sensitivityModifier = 0.1f;
        
        deltaAzimuth = (m_currentMousePosition.x - m_previousDrawMousePosition.x) * m_sensitivity * deltaTime * sensitivityModifier;
        m_cameraAzimuth += deltaAzimuth;
        
        deltaAltitude = (m_currentMousePosition.y - m_previousDrawMousePosition.y) * m_sensitivity * deltaTime * sensitivityModifier;
        m_cameraAltitude += deltaAltitude;
    }
    
    if (m_cameraAltitude > 90.0f)
    {
        m_cameraAltitude = 90.f;
    }
    else if (m_cameraAltitude < -90.0f)
    {
        m_cameraAltitude = -90.0f;
    }
    
    glm::mat4 azimuthViewTransform = glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraAzimuth), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 altitudeViewTransform = glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraAltitude), glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Determine translation
    glm::vec3 deltaTranslation = glm::vec3();
    
    if (m_isKeyPressed['W'])
    {
        deltaTranslation.z += m_sensitivity * deltaTime;
    }
    if (m_isKeyPressed['S'])
    {
        deltaTranslation.z -= m_sensitivity * deltaTime;
    }
    if (m_isKeyPressed['A'])
    {
        deltaTranslation.x += m_sensitivity * deltaTime;
    }
    if (m_isKeyPressed['D'])
    {
        deltaTranslation.x -= m_sensitivity * deltaTime;
    }
    
    // move cylinder light around
    float deltaHeight = 0.0f;
    float deltaAngle = 0.0f;
    if (m_isKeyPressed['I'])
    {
        deltaHeight += m_sensitivity * deltaTime * 2.0f;
    }
    if (m_isKeyPressed['K'])
    {
        deltaHeight -= m_sensitivity * deltaTime * 2.0f;
    }
    if (m_isKeyPressed['J'])
    {
        deltaAngle -= m_sensitivity * deltaTime / 20.0f;
    }
    if (m_isKeyPressed['L'])
    {
        deltaAngle += m_sensitivity * deltaTime / 20.0f;
    }
    
    m_light2Height += deltaHeight;
    m_light2Angle += deltaAngle;
    
    deltaTranslation = glm::vec3(glm::transpose(altitudeViewTransform * azimuthViewTransform) * glm::vec4(deltaTranslation, 0.0f));
    m_cameraPosition += deltaTranslation;

    glm::mat4 translationViewTransform = glm::translate(glm::mat4(1.0f), m_cameraPosition);
    
    view = altitudeViewTransform * azimuthViewTransform * translationViewTransform;
    
    glUniform4f(m_camera_pos, m_cameraPosition.x, m_cameraPosition.y, m_cameraPosition.z, 1.0f);
    glUniform4f(m_light_1_pos, m_cameraPosition.x, m_cameraPosition.y, m_cameraPosition.z, 1.0f);
    
    glm::mat4 rotateLight = glm::rotate(glm::mat4(1.0f), m_light2Angle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translateLight = glm::translate(glm::mat4(1.0f), glm::vec3(m_light2Radius, m_light2Height, 0.0f));
    glm::vec3 light2pos =  rotateLight * translateLight * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
    glUniform4f(m_light_2_pos, light2pos.x, light2pos.y, light2pos.z, 1.0f);

	glm::mat4 viewProjection = projection * view;
    glUniform1i(m_use_phong, m_shadingModel);

	glUniformMatrix4fv(m_uniform_viewProjection, 1, GL_FALSE, &viewProjection[0][0]);

	//DrawMesh(m_cow);
    
  //  DrawMesh(m_teapot);

    DrawMesh(m_teapot);
    
    m_previousDrawMousePosition = m_currentMousePosition;
    m_lastTime = time;
}

void Application::SetShadingModel(int shadingModel)
{
    m_shadingModel = (shadingModel + 1) % 2;
}

void Application::SetMaterial(int materialIndex)
{
    glUniform4f(m_mat_ambient, 0.025, 0.175, 0.05, 1.0);
    glUniform4f(m_mat_diffuse, 0.5, 0.7, 0.5, 1.0);
    glUniform4f(m_mat_specular, 0.1, 0.2, 0.1, 1.0);
    glUniform1f(m_mat_shininess, 1.0);
    glUniform4f(m_light_1_ambient, 0.1, 0.1, 0.1, 1.0);
    glUniform4f(m_light_1_diffuse, 0.4, 0.4, 0.4, 1.0);
    glUniform4f(m_light_1_specular, 0.2, 0.2, 0.2, 1.0);
    glUniform4f(m_light_2_ambient, 0.1, 0.1, 0.1, 1.0);
    glUniform4f(m_light_2_diffuse, 0.5, 0.5, 0.5, 1.0);
    glUniform4f(m_light_2_specular, 0.2, 0.2, 0.2, 1.0);
}

void Application::DrawMesh(MountainTerrainTile& object)
{
    glUniform1i(m_use_texture_map, 0);
	object.Draw();
}
