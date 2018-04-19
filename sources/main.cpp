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

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1024, 1024, "CS 470 Final Project", NULL, NULL);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	
    std::shared_ptr<Application> app = std::make_shared<Application>();
    glfwSetWindowUserPointer(window, (void*)app.get());
    
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
    app->SetMaterial(materialIndex);
    
    int shadingModel = 0;
    int lastShadingModel = 0;
    app->SetShadingModel(shadingModel);
    
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
        /* Poll for and process events */
        glfwPollEvents();
        
        auto current_timestamp = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed_time = (current_timestamp - start);
        
        if (materialIndex != lastMaterialIndex)
        {
            app->SetMaterial(materialIndex);
            lastMaterialIndex = materialIndex;
        }
        if (shadingModel != lastShadingModel)
        {
            app->SetShadingModel(shadingModel);
            lastShadingModel = shadingModel;
        }
        if (lightHeight != lastLightHeight)
        {
            app->SetLight2Height(lightHeight);
            lastLightHeight = lightHeight;
        }
        if (lightRadius != lastLightRadius)
        {
            app->SetLight2Radius(lightRadius);
            lastLightRadius = lightRadius;
        }
        if (lightAngle != lastLightAngle)
        {
            app->SetLight2Angle(lightAngle);
            lastLightAngle = lightAngle;
        }
        
        app->Draw(elapsed_time.count());
        
        if (firstPass)
        {
            firstPass = false;
            app->SetMaterial(0);
        }
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

	glfwTerminate();
	return 0;
}
