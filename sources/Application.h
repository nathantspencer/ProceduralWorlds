#pragma once

#include <Common/File/FileIO.h>
#include <Terrain/MountainTerrainTile.h>

#include <glm/glm.hpp>
#include <map>
#include <string>

class Application
{
public:
	Application();
	~Application();

	void Draw(float time, float aspect);
    
    void KeyCallback(int keycode, int event);
    void MouseButtonCallback(int buttoncode, int event);
    void CursorPosCallback(float x, float y);
    void ScrollCallback(float x, float y);
    
    void SetMaterial(int materialIndex);
    void SetShadingModel(int shadingModel);
    
    void SetLight2Radius(float radius);
    void SetLight2Angle(float angle);
    void SetLight2Height(float height);

private:
    
	void DrawMesh(MountainTerrainTile* object);

    std::unique_ptr<MountainTerrainTile> m_terrainTile;

	unsigned int m_program;
    unsigned int m_uniform_offset;
	unsigned int m_uniform_transform;
	unsigned int m_uniform_viewProjection;
	unsigned int m_uniform_color;
	unsigned int m_uniform_ambient;
	unsigned int m_uniform_light_pos;
    
    float   m_sensitivity;
    float   m_lastTime;
    
    glm::vec3   m_cameraPosition;
    float       m_cameraAzimuth;
    float       m_cameraAltitude;
    
    unsigned int m_camera_pos;
    unsigned int m_use_phong;
    unsigned int m_mat_ambient;
    unsigned int m_mat_specular;
    unsigned int m_mat_diffuse;
    unsigned int m_mat_shininess;
    unsigned int m_use_texture_map;
    
    unsigned int m_light_1_pos;
    unsigned int m_light_1_ambient;
    unsigned int m_light_1_diffuse;
    unsigned int m_light_1_specular;
    
    unsigned int m_light_2_pos;
    unsigned int m_light_2_ambient;
    unsigned int m_light_2_diffuse;
    unsigned int m_light_2_specular;
    
    float   m_light2Height;
    float   m_light2Radius;
    float   m_light2Angle;
    
    int m_shadingModel;
    
    glm::vec2   m_currentMousePosition;
    glm::vec2   m_previousDrawMousePosition;
    
    std::map<char, bool>    m_isKeyPressed;
    bool                    m_isMouseButtonPressed;
};
