#include "Application.h"

#include <Shading/ProgramLibrary.h>
#include <Shading/ShaderIO.h>
#include <Terrain/MountainTerrainTile.h>

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

ProgramLibrary programLibrary;

Application::Application()
{
	gl3wInit();
    
	const char* OpenGLversion = (const char*) glGetString(GL_VERSION);
	const char* GLSLversion = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("OpenGL %s\nGLSL: %s\n\n", OpenGLversion, GLSLversion);

	glClearColor(0.30f, 0.45f, 0.68f, 1.00f);
	glClearDepth(1.0f);

    GLuint mountainTerrainVS = ShaderIO::CompileShader("../../shaders/mountainTerrain.vs.glsl");
    GLuint mountainTerrainFS = ShaderIO::CompileShader("../../shaders/mountainTerrain.fs.glsl");
    std::vector<GLuint> mountainTerrainShaders = { mountainTerrainVS, mountainTerrainFS };
    programLibrary.AddProgram("mountainTerrain", mountainTerrainShaders);
    programLibrary.SetActiveProgram("mountainTerrain");
    GLuint activeProgram = programLibrary.GetActiveProgram();

	m_uniform_transform = glGetUniformLocation(activeProgram, "u_transform");
	m_uniform_viewProjection = glGetUniformLocation(activeProgram, "u_viewProjection");
	
	m_uniform_color = glGetUniformLocation(activeProgram, "u_color");
	m_uniform_ambient = glGetUniformLocation(activeProgram, "u_ambient");
    m_uniform_light_pos = glGetUniformLocation(activeProgram, "u_light_pos");
    
    m_uniform_offset = glGetUniformLocation(activeProgram, "u_offset");
    m_mat_ambient = glGetUniformLocation(activeProgram, "u_mat_ambient");
    m_mat_diffuse = glGetUniformLocation(activeProgram, "u_mat_diffuse");
    m_mat_specular = glGetUniformLocation(activeProgram, "u_mat_specular");
    m_mat_shininess = glGetUniformLocation(activeProgram, "u_mat_shininess");
    
    m_light_1_pos = glGetUniformLocation(activeProgram, "u_light_1_pos");
    m_light_1_ambient = glGetUniformLocation(activeProgram, "u_light_1_ambient");
    m_light_1_diffuse = glGetUniformLocation(activeProgram, "u_light_1_diffuse");
    m_light_1_specular = glGetUniformLocation(activeProgram, "u_light_1_specular");
    
    m_light_2_pos = glGetUniformLocation(activeProgram, "u_light_2_pos");
    m_light_2_ambient = glGetUniformLocation(activeProgram, "u_light_2_ambient");
    m_light_2_diffuse = glGetUniformLocation(activeProgram, "u_light_2_diffuse");
    m_light_2_specular = glGetUniformLocation(activeProgram, "u_light_2_specular");
    
    m_use_phong = glGetUniformLocation(activeProgram, "u_use_phong");
    m_use_texture_map = glGetUniformLocation(activeProgram, "u_use_texture_map");
    m_camera_pos = glGetUniformLocation(activeProgram, "u_camera_pos");

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
