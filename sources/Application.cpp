// Nathan Spencer
// CS 470 Assignment 3

// Toggle orbit/free camera using "o" key
// You begin in free camera mode

#include "Application.h"
#include "Object.h"
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

	const char* OpenGLversion = (const char*)glGetString(GL_VERSION);
	const char* GLSLversion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("OpenGL %s GLSL: %s", OpenGLversion, GLSLversion);

	glClearColor(0.30f, 0.45f, 0.68f, 1.00f);
	glClearDepth(1.0f);

	const char* vertex_shader_src = R"(
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
    
		attribute vec3 a_position;
		attribute vec3 a_normal;
        attribute vec2 a_uv;

		uniform mat4 u_transform;
		uniform mat4 u_viewProjection;

		varying vec4 v_normal;
		varying vec4 v_pos;
        varying vec2 v_uv;
    
        uniform vec2 u_offset;
        varying float height;
    
        float NOISE_GRANULARITY = 0.01;
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
            vec2 st = vec2(a_position.x + 2.0 * u_offset.x, a_position.z + 2.0 * u_offset.y);
            st *= NOISE_GRANULARITY;
            vec3 heightMappedPos = a_position;
            heightMappedPos.y += ridge(st);
            
            height = heightMappedPos.y;
            
            v_normal = normalize(u_transform * vec4(a_normal, 0.0));
            v_pos = u_transform * vec4(heightMappedPos, 1.0);
            v_uv = a_uv;
            gl_Position = u_viewProjection * v_pos;
        }
	)";

	const char* fragment_shader_src = R"(
    
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
        uniform sampler2D u_texture;
    
        varying vec4 v_normal;
        varying vec4 v_pos;
        varying vec2 v_uv;
    
        varying float height;

		void main()
		{
            if (u_use_phong)
            {
                vec3 ambient_component = u_mat_ambient.xyz;
                
                vec3 v = normalize(u_camera_pos.xyz - v_pos.xyz);
                vec3 l1 = normalize(u_light_1_pos.xyz - v_pos.xyz);
                vec3 l2 = normalize(u_light_2_pos.xyz - v_pos.xyz);
                vec3 r1 = (v_normal.xyz * 2.0 * dot(l1, v_normal.xyz)) - l1;
                vec3 r2 = (v_normal.xyz * 2.0 * dot(l2, v_normal.xyz)) - l2;
                
                vec3 mat_diffuse = u_mat_diffuse.xyz;
                if (u_use_texture_map)
                {
                   mat_diffuse = texture2D(u_texture, v_uv.xy).rgb;
                }

                vec3 diffuse_light_1 = u_light_1_diffuse.xyz * mat_diffuse.xyz * max(dot(l1, v_normal.xyz), 0.0);
                vec3 diffuse_light_2 = u_light_2_diffuse.xyz * mat_diffuse.xyz * max(dot(l2, v_normal.xyz), 0.0);
                vec3 diffuse_component = diffuse_light_1 + diffuse_light_2;
                
                vec3 specular_light_1 = u_light_1_specular.xyz * u_mat_specular.xyz * pow(max(dot(r1, v), 0.0), u_mat_shininess);
                vec3 specular_light_2 = u_light_2_specular.xyz * u_mat_specular.xyz * pow(max(dot(r2, v), 0.0), u_mat_shininess);
                vec3 specular_component = specular_light_1 + specular_light_2;
                
                gl_FragColor = vec4(ambient_component + diffuse_component + specular_component + (abs(height) / 400.0), 1.0);
            }
            else
            {
                if (u_use_texture_map)
                {
                    vec3 v = normalize(u_camera_pos.xyz - v_pos.xyz);
                    vec3 l1 = normalize(u_light_1_pos.xyz - v_pos.xyz);
                    vec3 l2 = normalize(u_light_2_pos.xyz - v_pos.xyz);
                    
                    vec3 mat_diffuse = texture2D(u_texture, v_uv.xy).rgb;
                    vec3 diffuse_light_1 = u_light_1_diffuse.xyz * mat_diffuse.xyz * max(dot(l1, v_normal.xyz), 0.0);
                    vec3 diffuse_light_2 = u_light_2_diffuse.xyz * mat_diffuse.xyz * max(dot(l2, v_normal.xyz), 0.0);
                    vec3 diffuse_component = diffuse_light_1 + diffuse_light_2;
                    
                    gl_FragColor = gl_Color + vec4(diffuse_component, 1.0);
                }
                else
                {
                    gl_FragColor = gl_Color;
                }
            }
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

	m_attrib_pos = glGetAttribLocation(m_program, "a_position");
	m_attrib_normal = glGetAttribLocation(m_program, "a_normal");
    m_attrib_uv = glGetAttribLocation(m_program, "a_uv");

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

	m_cow.Load("../../cow.obj");
	m_teapot.Load("../../terrainTile.obj");
    
    m_cameraPosition = glm::vec3(0.0f, 0.0f, -50.0f);
    m_orbitCameraPosition = m_cameraPosition;
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
    m_useOrbitView = false;
    m_orbitRadius = 50.0f;
    m_deltaOrbitRadius = 0.0f;
    m_orbitCameraAzimuth = 0.0f;
    m_orbitCameraAltitude = 0.0f;
    
    m_light2Angle = 0.0f;
    m_light2Height = 50.0f;
    m_light2Radius = 50.0f;
}

void Application::KeyCallback(int keycode, int event)
{
    char character = char(keycode);
    if (character == 'O' && event == 1)
    {
        m_useOrbitView = !m_useOrbitView;
    }
    else
    {
        m_isKeyPressed[character] = bool(event);
    }
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
    if (m_useOrbitView)
    {
        m_deltaOrbitRadius += y;
    }
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

void Application::Draw(float time)
{
    float deltaTime = time - m_lastTime;
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glUseProgram(m_program);

	glm::mat4 transform = glm::mat4(1.0);
	glUniformMatrix4fv(m_uniform_transform, 1, GL_FALSE, &transform[0][0]);

	glm::mat4 projection = glm::perspective(45.0f, 1.0f, 10.0f, 10000.0f);
    glm::mat4 view = glm::mat4();
    
    // Free flying camera
    if(!m_useOrbitView)
    {
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
    }
    
    // Orbit view
    else
    {
        // Determine radius
        m_orbitRadius -= m_deltaOrbitRadius * m_sensitivity * deltaTime;
        m_deltaOrbitRadius = 0.0f;
        if(m_orbitRadius < 0.0f)
        {
            m_orbitRadius = 0.0f;
        }
        
        // Determine azimuth and altitude angles
        float deltaAzimuth;
        float deltaAltitude;
        
        if (m_isMouseButtonPressed)
        {
            float sensitivityModifier = 0.1f;
            
            deltaAzimuth = (m_currentMousePosition.x - m_previousDrawMousePosition.x) * m_sensitivity * deltaTime * sensitivityModifier;
            m_orbitCameraAzimuth += deltaAzimuth;
            
            deltaAltitude = (m_currentMousePosition.y - m_previousDrawMousePosition.y) * m_sensitivity * deltaTime * sensitivityModifier;
            m_orbitCameraAltitude += deltaAltitude;
        }
        
        if (m_orbitCameraAltitude > 90.0f)
        {
            m_orbitCameraAltitude = 90.f;
        }
        else if (m_orbitCameraAltitude < -90.0f)
        {
            m_orbitCameraAltitude = -90.0f;
        }
        
        glm::mat4 azimuthViewTransform = glm::rotate(glm::mat4(1.0f), glm::radians(m_orbitCameraAzimuth), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 altitudeViewTransform = glm::rotate(glm::mat4(1.0f), glm::radians(m_orbitCameraAltitude), glm::vec3(1.0f, 0.0f, 0.0f));
        
        m_orbitCameraPosition = glm::vec3(0.0f, 0.0f, -m_orbitRadius);
        glm::mat4 translationViewTransform = glm::translate(glm::mat4(1.0f), m_orbitCameraPosition);
        view = translationViewTransform * altitudeViewTransform * azimuthViewTransform;
    }
    
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

    for (int i = -20; i < 20; ++i)
    {
        for (int j = -20; j < 20; ++j)
        {
            glUniform2f(m_uniform_offset, float(i), float(j));
            transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 20.0f, -10.0f, j * 20.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
            glUniformMatrix4fv(m_uniform_transform, 1, GL_FALSE, &transform[0][0]);
            
            DrawMesh(m_teapot);
        }
    }

    
    m_previousDrawMousePosition = m_currentMousePosition;
    m_lastTime = time;
}

void Application::SetShadingModel(int shadingModel)
{
    m_shadingModel = (shadingModel + 1) % 2;
}

void Application::SetMaterial(int materialIndex)
{
    // shiny red balloon
    if (materialIndex == 2)
    {
        glUniform4f(m_mat_ambient, 0.6, 0.2, 0.2, 1.0);
        glUniform4f(m_mat_diffuse, 0.9, 0.1, 0.1, 1.0);
        glUniform4f(m_mat_specular, 0.8, 0.8, 0.8, 1.0);
        glUniform1f(m_mat_shininess, 80.0);
        glUniform4f(m_light_1_ambient, 0.2, 0.2, 0.2, 1.0);
        glUniform4f(m_light_1_diffuse, 0.6, 0.6, 0.6, 1.0);
        glUniform4f(m_light_1_specular, 1.0, 1.0, 1.0, 1.0);
        glUniform4f(m_light_2_ambient, 0.2, 0.2, 0.2, 1.0);
        glUniform4f(m_light_2_diffuse, 0.6, 0.6, 0.6, 1.0);
        glUniform4f(m_light_2_specular, 1.0, 1.0, 1.0, 1.0);
    }
    
    // dream surface
    else if (materialIndex == 1)
    {
        glUniform4f(m_mat_ambient, 0.5, 0.4, 0.2, 1.0);
        glUniform4f(m_mat_diffuse, 0.4, 0.7, 0.4, 1.0);
        glUniform4f(m_mat_specular, 0.0, 0.2, 0.3, 1.0);
        glUniform1f(m_mat_shininess, 5.0);
        glUniform4f(m_light_1_ambient, 0.3, 0.2, 0.2, 1.0);
        glUniform4f(m_light_1_diffuse, 0.3, 0.8, 0.3, 1.0);
        glUniform4f(m_light_1_specular, 0.75, 0.75, 1.0, 1.0);
        glUniform4f(m_light_2_ambient, 0.3, 0.2, 0.2, 1.0);
        glUniform4f(m_light_2_diffuse, 0.3, 0.8, 0.3, 1.0);
        glUniform4f(m_light_2_specular, 0.75, 0.75, 1.0, 1.0);
    }
    
    // matte charcoal
    else if (materialIndex == 0)
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
}

void Application::DrawMesh(Object& object)
{
    glUniform1i(m_use_texture_map, 0);
    
	object.Bind();

	glEnableVertexAttribArray(m_attrib_pos);
    glVertexAttribPointer(m_attrib_pos, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), ToVoidPointer(0));

	glEnableVertexAttribArray(m_attrib_normal);
    glVertexAttribPointer(m_attrib_normal, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), ToVoidPointer(sizeof(glm::vec3)));
    
    glEnableVertexAttribArray(m_attrib_uv);
    glVertexAttribPointer(m_attrib_uv, 2, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), ToVoidPointer(2 * sizeof(glm::vec3)));

	object.Draw();

	glDisableVertexAttribArray(m_attrib_pos);
	glDisableVertexAttribArray(m_attrib_normal);
    glDisableVertexAttribArray(m_attrib_uv);

	object.UnBind();
}
