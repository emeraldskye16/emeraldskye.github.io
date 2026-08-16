///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>    

// declaration of the global variables and defines
namespace
{
	// Variables for window width and height
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 800;
	const char* g_ViewName = "view";
	const char* g_ProjectionName = "projection";

	// camera object used for viewing and interacting with
	// the 3D scene
	Camera* g_pCamera = nullptr;

	// these variables are used for mouse movement processing
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	// time between current frame and last frame
	float gDeltaTime = 0.0f; 
	float gLastFrame = 0.0f;

	// the following variable is false when orthographic projection
	// is off and true when it is on
	bool bOrthographicProjection = false;
}

/***********************************************************
 *  ViewManager()
 *
 *  The constructor for the class
 ***********************************************************/
ViewManager::ViewManager(
	ShaderManager *pShaderManager)
{
	// initialize the member variables
	m_pShaderManager = pShaderManager;
	m_pWindow = NULL;
	g_pCamera = new Camera();
	// default camera view parameters
	g_pCamera->Position = glm::vec3(0.0f, 5.0f, 30.0f);
	g_pCamera->Front = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
	g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
	g_pCamera->Zoom = 45;
}

/***********************************************************
 *  ~ViewManager()
 *
 *  The destructor for the class
 ***********************************************************/
ViewManager::~ViewManager()
{
	// free up allocated memory
	m_pShaderManager = NULL;
	m_pWindow = NULL;
	if (NULL != g_pCamera)
	{
		delete g_pCamera;
		g_pCamera = NULL;
	}
}

/***********************************************************
 *  CreateDisplayWindow()
 *
 *  This method is used to create the main display window.
 ***********************************************************/
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
	GLFWwindow* window = nullptr;

	// try to create the displayed OpenGL window
	window = glfwCreateWindow(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		windowTitle,
		NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	// tell GLFW to capture all mouse events
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// this callback is used to receive mouse moving events
	glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);

	// Receive mouse scroll wheel events
	glfwSetScrollCallback(window, &ViewManager::Mouse_Scroll_Callback);

	glfwSetFramebufferSizeCallback(window, ViewManager::Framebuffer_Size_Callback);

	// tell GLFW to capture all mouse events
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// enable blending for supporting tranparent rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pWindow = window;

	return(window);
}

void ViewManager::AttachWindow(GLFWwindow* window)
{
	if (window == NULL) return;

	// store window
	m_pWindow = window;

	// register callbacks on provided window
	glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);
	glfwSetScrollCallback(window, &ViewManager::Mouse_Scroll_Callback);
	glfwSetFramebufferSizeCallback(window, ViewManager::Framebuffer_Size_Callback);

	// capture mouse and enable blending for alpha-based materials
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ViewManager::Framebuffer_Size_Callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

/***********************************************************
 *  Mouse_Position_Callback()
 *
 *  This method is automatically called from GLFW whenever
 *  the mouse is moved within the active GLFW display window.
 ***********************************************************/
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
	if (gFirstMouse)
	{
		gLastX = xMousePos;
		gLastY = yMousePos;
		gFirstMouse = false;
	}

	// calculate the X offset and Y offset values for moving the 3D camera accordingly
	float xOffset = xMousePos - gLastX;
	float yOffset = gLastY - yMousePos; // reversed since y-coordinates go from bottom to top

	// set the current positions into the last position variables
	gLastX = xMousePos;
	gLastY = yMousePos;

	// move the 3D camera according to the calculated offsets
	g_pCamera->ProcessMouseMovement(xOffset, yOffset);
}
/***********************************************************
 *  Mouse_Scroll_Callback()
 *
 *  Called automatically whenever the mouse wheel is scrolled.
 *  We will use it to adjust camera MOVEMENT speed.
 ***********************************************************/
void ViewManager::Mouse_Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (g_pCamera == NULL)
		return;

	if (!bOrthographicProjection)
	{
		// Perspective: use camera's FOV-style zoom
		g_pCamera->ProcessMouseScroll((float)yOffset);
	}
	else
	{
		// Orthographic: use Zoom as ortho "half-height" size in world units
		g_pCamera->Zoom -= (float)yOffset;       // scroll up => smaller => zoom in
		if (g_pCamera->Zoom < 1.0f)  g_pCamera->Zoom = 1.0f;
		if (g_pCamera->Zoom > 50.0f) g_pCamera->Zoom = 50.0f;
	}
}

/***********************************************************
 *  ProcessKeyboardEvents()
 *
 *  This method is called to process any keyboard events
 *  that may be waiting in the event queue.
 ***********************************************************/
void ViewManager::ProcessKeyboardEvents()
{
	// close the window if the escape key has been pressed
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_pWindow, true);
	}
	// if the camera object is null, then exit this method
	if (NULL == g_pCamera)
	{
		return;
	}

	// process camera zooming in and out
	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(FORWARD, gDeltaTime);
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(BACKWARD, gDeltaTime);
	}

	// process camera panning left and right
	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(LEFT, gDeltaTime);
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(RIGHT, gDeltaTime);
	}

	// process camera moving downward and upward
	if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(UP, gDeltaTime);
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(DOWN, gDeltaTime);
	}

	// Tap toggles (edge-trigger)
	static bool oWasDown = false;
	static bool pWasDown = false;

	bool oIsDown = glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS;
	bool pIsDown = glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS;


	if (oIsDown && !oWasDown)
	{
		bOrthographicProjection = true;

		// Look directly at the object (assumes object centered at origin)
		glm::vec3 target(0.0f, 0.0f, 0.0f);

		// Place camera straight in front of the object so the view is "flat"
		g_pCamera->Position = glm::vec3(0.0f, 0.0f, 20.0f);
		g_pCamera->Front = glm::normalize(target - g_pCamera->Position);
		g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);

		// Use Zoom as orthographic size (half-height of view volume)
		g_pCamera->Zoom = 10.0f;
	}

	if (pIsDown && !pWasDown)
	{
		bOrthographicProjection = false;

		// Perspective camera defaults (restore what you want)
		g_pCamera->Position = glm::vec3(0.0f, 5.0f, 12.0f);
		g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
		g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);

		// Restore a typical FOV in degrees (your constructor uses 100 which is very wide)
		g_pCamera->Zoom = 45.0f;
	}

	oWasDown = oIsDown;
	pWasDown = pIsDown;
}

/***********************************************************
 *  PrepareSceneView()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void ViewManager::PrepareSceneView()
{
	if (m_pShaderManager) m_pShaderManager->use();

	glm::mat4 view;
	glm::mat4 projection;

	// per-frame timing
	float currentFrame = glfwGetTime();
	gDeltaTime = currentFrame - gLastFrame;
	gLastFrame = currentFrame;

	// process any keyboard events that may be waiting in the 
	// event queue
	ProcessKeyboardEvents();

	// get the current view matrix from the camera
	view = g_pCamera->GetViewMatrix();

	// Compute aspect ratio from framebuffer size (recommended)
	int fbWidth = 0, fbHeight = 0;
	glfwGetFramebufferSize(m_pWindow, &fbWidth, &fbHeight);
	float aspect = (fbHeight == 0) ? 1.0f : (float)fbWidth / (float)fbHeight;

	if (!bOrthographicProjection)
	{
		// Perspective projection (3D)
		projection = glm::perspective(
			glm::radians(g_pCamera->Zoom),
			aspect,
			0.1f,
			100.0f
		);
	}
	else
	{
		// Orthographic projection (2D-like)
		// Use camera->Zoom as "half-height" of the ortho box
		float orthoHalfHeight = g_pCamera->Zoom;
		float orthoHalfWidth = orthoHalfHeight * aspect;

		projection = glm::ortho(
			-orthoHalfWidth, orthoHalfWidth,
			-orthoHalfHeight, orthoHalfHeight,
			0.1f,
			100.0f
		);
	}

	// if the shader manager object is valid
	if (NULL != m_pShaderManager)
	{
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ViewName, view);
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ProjectionName, projection);
		// set the view position of the camera into the shader for proper rendering
		m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
	}
}