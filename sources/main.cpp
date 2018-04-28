#include "Application.h"

#include <GLFW/glfw3.h>

#include <memory>
#include <chrono>

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	glfwInit();

	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_RED_BITS, 8);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(1280, 800, "ProceduralWorlds: Mountains", NULL, NULL);
	glfwMakeContextCurrent(window);
	
    std::shared_ptr<Application> application = std::make_shared<Application>();
    glfwSetWindowUserPointer(window, (void*) application.get());
    
    glfwSetKeyCallback(window, [](GLFWwindow* window, int keycode, int scancode, int event, int modifiers)
    {
        static_cast<Application*>(glfwGetWindowUserPointer(window))->KeyCallback(keycode, event);
    });
    
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int buttoncode, int event, int modifiers)
    {
        static_cast<Application*>(glfwGetWindowUserPointer(window))->MouseButtonCallback(buttoncode, event);
    });
    
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y)
    {
        static_cast<Application*>(glfwGetWindowUserPointer(window))->CursorPosCallback(x, y);
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y)
    {
        static_cast<Application*>(glfwGetWindowUserPointer(window))->ScrollCallback(x, y);
    });

    auto start = std::chrono::steady_clock::now();
    
    int materialIndex = 0;
    int lastMaterialIndex = 0;
    application->SetMaterial(materialIndex);
    
    int shadingModel = 0;
    int lastShadingModel = 0;
    application->SetShadingModel(shadingModel);
    
    float lightHeight = 50.0f;
    float lastLightHeight = 50.0f;
    
    float lightRadius = 50.0f;
    float lastLightRadius = 50.0f;
    
    float lightAngle = 0.0f;
    float lastLightAngle = 0.0f;
    
    bool firstPass = true;
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        auto current_timestamp = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed_time = (current_timestamp - start);
        
        if (materialIndex != lastMaterialIndex)
        {
            application->SetMaterial(materialIndex);
            lastMaterialIndex = materialIndex;
        }
        if (shadingModel != lastShadingModel)
        {
            application->SetShadingModel(shadingModel);
            lastShadingModel = shadingModel;
        }
        if (lightHeight != lastLightHeight)
        {
            application->SetLight2Height(lightHeight);
            lastLightHeight = lightHeight;
        }
        if (lightRadius != lastLightRadius)
        {
            application->SetLight2Radius(lightRadius);
            lastLightRadius = lightRadius;
        }
        if (lightAngle != lastLightAngle)
        {
            application->SetLight2Angle(lightAngle);
            lastLightAngle = lightAngle;
        }
        
        application->Draw(elapsed_time.count());
        
        if (firstPass)
        {
            firstPass = false;
            application->SetMaterial(0);
        }
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

	glfwTerminate();
	return 0;
}
