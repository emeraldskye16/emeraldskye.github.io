#include <iostream>         // error handling and output
#include <cstdlib>          // EXIT_FAILURE

#include <GL/glew.h>        // GLEW library
#include "GLFW/glfw3.h"     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneManager.h"
#include "ViewManager.h"
#include "ShapeMeshes.h"
#include "ShaderManager.h"

// Namespace for declaring global variables
namespace
{
	// Macro for window title
	const char* const WINDOW_TITLE = "7-1 FinalProject and Milestones"; 

	// Main GLFW window
	GLFWwindow* g_Window = nullptr;

	// scene manager object for managing the 3D scene prepare and render
	SceneManager* g_SceneManager = nullptr;
	// shader manager object for dynamic interaction with the shader code
	ShaderManager* g_ShaderManager = nullptr;
	// view manager object for managing the 3D view setup and projection to 2D
	ViewManager* g_ViewManager = nullptr;
}

// Function declarations - all functions that are called manually
// need to be pre-declared at the beginning of the source code.
bool InitializeGLFW();
bool InitializeGLEW();


/***********************************************************
 *  main(int, char*)
 *
 *  This function gets called after the application has been
 *  launched.
 ***********************************************************/
int main(int argc, char* argv[])
{
	// if GLFW fails initialization, then terminate the application
	if (InitializeGLFW() == false)
	{
		return(EXIT_FAILURE);
	}

	// try to create a new shader manager object
	g_ShaderManager = new ShaderManager();
	// try to create a new view manager object
	g_ViewManager = new ViewManager(
		g_ShaderManager);

	// try to create the main display window
	g_Window = g_ViewManager->CreateDisplayWindow(WINDOW_TITLE);

	// Attach the already-created GLFW window to the ViewManager so it can register callbacks
	g_ViewManager->AttachWindow(g_Window);

	// if GLEW fails initialization, then terminate the application
	if (InitializeGLEW() == false)
	{
		return(EXIT_FAILURE);
	}

	// Ensure depth testing and a visible clear color are enabled
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Set a visible clear color so you can tell rendering is occurring
	glClearColor(0.53f, 0.80f, 0.92f, 1.0f); // light sky-blue

	// Make sure the viewport matches the window framebuffer size
	int fbWidth = 0, fbHeight = 0;
	glfwGetFramebufferSize(g_Window, &fbWidth, &fbHeight);
	glViewport(0, 0, (fbWidth > 0) ? fbWidth : 800, (fbHeight > 0) ? fbHeight : 600);

	// load the shader code from the external GLSL files
	const char* vertPath = "../../Utilities/shaders/vertexShader.glsl";
	const char* fragPath = "../../Utilities/shaders/fragmentShader.glsl";

	if (g_ShaderManager->LoadShaders(vertPath, fragPath) == 0)
	{
		std::cerr << "ERROR: shader load failed. See compile/link output above.\n";
		return EXIT_FAILURE;
	}
	std::cout << "Shader program ID: " << g_ShaderManager->m_programID << "\n";

	g_ShaderManager->use(); // make the shader program active

	GLint locModel = glGetUniformLocation(g_ShaderManager->m_programID, "model");
	GLint locView = glGetUniformLocation(g_ShaderManager->m_programID, "view");
	GLint locProj = glGetUniformLocation(g_ShaderManager->m_programID, "projection");
	std::cout << "Uniform locations: model=" << locModel << ", view=" << locView << ", projection=" << locProj << "\n";

	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
		std::cerr << "GL error after shader use: 0x" << std::hex << err << std::dec << "\n";

	// try to create a new scene manager object and prepare the 3D scene
	g_SceneManager = new SceneManager(g_ShaderManager);
	g_SceneManager->PrepareScene();

	// DEBUG: show wireframe and avoid hiding back faces
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);

	// loop will keep running until the application is closed 
	// or until an error has occurred
	while (!glfwWindowShouldClose(g_Window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// update camera/view/projection and push to shader
		g_ViewManager->PrepareSceneView();

		// Use shader program.
		g_ShaderManager->use();

		// Render the scene.
		g_SceneManager->RenderScene();

		// Swap buffers and poll events.
		glfwSwapBuffers(g_Window);
		glfwPollEvents();
	}

	// clear the allocated manager objects from memory
	if (NULL != g_SceneManager)
	{
		delete g_SceneManager;
		g_SceneManager = NULL;
	}
	if (NULL != g_ViewManager)
	{
		delete g_ViewManager;
		g_ViewManager = NULL;
	}
	if (NULL != g_ShaderManager)
	{
		delete g_ShaderManager;
		g_ShaderManager = NULL;
	}

	// Terminates the program successfully
	exit(EXIT_SUCCESS); 
}

/***********************************************************
 *	InitializeGLFW()
 * 
 *  This function is used to initialize the GLFW library.   
 ***********************************************************/
bool InitializeGLFW()
{
	// GLFW: initialize and configure library
	// --------------------------------------
	glfwInit();

#ifdef __APPLE__
	// set the version of OpenGL and profile to use
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	// set the version of OpenGL and profile to use
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	// GLFW: end -------------------------------

	return(true);
}

/***********************************************************
 *	InitializeGLEW()
 *
 *  This function is used to initialize the GLEW library.
 ***********************************************************/
bool InitializeGLEW()
{
	// GLEW: initialize
	// -----------------------------------------
	GLenum GLEWInitResult = GLEW_OK;

	// try to initialize the GLEW library
	GLEWInitResult = glewInit();
	if (GLEW_OK != GLEWInitResult)
	{
		std::cerr << glewGetErrorString(GLEWInitResult) << std::endl;
		return false;
	}
	// GLEW: end -------------------------------

	// Displays a successful OpenGL initialization message
	std::cout << "INFO: OpenGL Successfully Initialized\n";
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << "\n" << std::endl;

	return(true);
}