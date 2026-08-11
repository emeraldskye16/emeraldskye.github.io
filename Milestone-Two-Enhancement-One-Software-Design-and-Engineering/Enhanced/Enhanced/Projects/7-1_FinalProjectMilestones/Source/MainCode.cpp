#include <cstdlib>
#include <filesystem>
namespace fs = std::filesystem;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneManager.h"
#include "ViewManager.h"
#include "ShaderManager.h"
#include "LighthouseObject.h"
#include "ShapeMeshes.h"

namespace
{
    const char* const WINDOW_TITLE = "CS330 Enhanced Lighthouse Scene";

    GLFWwindow* g_Window = nullptr;
    SceneManager* g_SceneManager = nullptr;
    ShaderManager* g_ShaderManager = nullptr;
    ViewManager* g_ViewManager = nullptr;
}

// Initialize GLFW and configure OpenGL context.
bool InitializeGLFW()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    return true;
}

void GLFWErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << '\n';
}

int main()
{
    std::cout << "Running from: " << fs::current_path() << "\n";

    // Set the GLFW error callback.
    glfwSetErrorCallback(GLFWErrorCallback);

    // Initialize GLFW.
    if (!InitializeGLFW())
        return EXIT_FAILURE;

    // Create ShaderManager once
    g_ShaderManager = new ShaderManager();

    // Create view manager (camera + window creation)
    g_ViewManager = new ViewManager(g_ShaderManager);

    // Create the display window THROUGH ViewManager
    g_Window = g_ViewManager->CreateDisplayWindow(WINDOW_TITLE);

    if (!g_Window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Initialize GLEW AFTER making the context current.
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW\n";
        return EXIT_FAILURE;
    }

    // Enable depth testing.
    glEnable(GL_DEPTH_TEST);

    // Build shader path relative to Debug folder.
    fs::path shaderFolder = fs::current_path()
        / ".."
        / ".."
        / ".."
        / "Utilities"
        / "shaders";

    fs::path vertexShaderPath = shaderFolder / "vertexShader.glsl";
    fs::path fragmentShaderPath = shaderFolder / "fragmentShader.glsl";

    std::cout << "Loading shaders from: " << shaderFolder << "\n";
    std::cout << "Vertex exists? " << fs::exists(vertexShaderPath) << "\n";
    std::cout << "Fragment exists? " << fs::exists(fragmentShaderPath) << "\n";

    // Load shaders into the single ShaderManager instance
    if (!g_ShaderManager->LoadShaders(
        vertexShaderPath.string().c_str(),
        fragmentShaderPath.string().c_str()))
    {
        std::cerr << "Failed to load shaders\n";
        return EXIT_FAILURE;
    }

    // Make the shader program active before uploading per-scene uniforms
    g_ShaderManager->use();

    // Create scene manager and prepare scene (textures, materials, etc.)
    g_SceneManager = new SceneManager(g_ShaderManager);
    g_SceneManager->PrepareScene();

    // Upload lighting/material uniforms now that the shader program is active
    g_SceneManager->SetupSceneLights();

    // Main render loop.
    while (!glfwWindowShouldClose(g_Window))
    {
        glEnable(GL_DEPTH_TEST);

        // Clear screen each frame
        glClearColor(0.25f, 0.35f, 0.45f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update camera and matrices (this calls shader->use() again)
        g_ViewManager->PrepareSceneView();

        // Draw scene
        g_SceneManager->RenderScene();

        // Display completed frame
        glfwSwapBuffers(g_Window);

        // Process keyboard/mouse events
        glfwPollEvents();
    }

    // Cleanup.
    delete g_SceneManager;
    delete g_ShaderManager;
    delete g_ViewManager;

    glfwTerminate();
    return EXIT_SUCCESS;
}