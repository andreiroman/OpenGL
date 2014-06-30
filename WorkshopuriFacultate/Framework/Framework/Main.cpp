#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "common.h"
#include "Utils.h"
#include "SpriteManager.h"
#include "TextRender.h"
#include "Player.h"
#include "Background.h"
#include "EntityManager.h"

int g_gl_width = 800;
int g_gl_height = 600;

GLFWwindow* window = NULL;

#define WHEIGHT 600
#define WWIDTH 800
void glfw_error_callback(int error, const char* description) {
	ErrorLog("GLFW ERROR: code %i msg: %s\n", error, description);
}

// a call-back function
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
	g_gl_width = width;
	g_gl_height = height;

	/* update any perspective matrices used here */
}

void _update_fps_counter(GLFWwindow* window) 
{
	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	if (elapsed_seconds > 0.25) {
		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];
		sprintf(tmp, "opengl @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count++;
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	assert(RestartLog());

	GameLog("starting GLFW\n%s\n", glfwGetVersionString());
	// start GL context and O/S window using the GLFW helper library

	// register the error call-back function that we wrote, above
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit()) {
		GameLog("ERROR: could not start GLFW3\n");
		return 1;
	}


	
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWmonitor* mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* vmode = glfwGetVideoMode(mon);

	window = glfwCreateWindow(WWIDTH /*vmode->width*/, WHEIGHT/*vmode->height*/, "GL Framework", /*mon*/NULL, NULL);
	if (!window) {
		GameLog("ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	LogOpenGLParams();

	glfwSetWindowSizeCallback(window, glfw_window_size_callback);

	glfwMakeContextCurrent(window);
	glViewport(0, 0, g_gl_width, g_gl_height);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	GameLog("Renderer: %s\n", renderer);
	GameLog("OpenGL version supported %s\n", version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glDisable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	TextRender txtrndr;
	CSpriteManager::Get()->Init();
	CPlayer * playerEntity = new CPlayer();
	playerEntity->Init();

	
	CBackground* backgroundEntity = new CBackground();
	backgroundEntity->Init();

	CEntityManager::Get()->AddEntity(playerEntity);
	CEntityManager::Get()->AddEntity(backgroundEntity);

	txtrndr.Init();
	txtrndr.LoadFont("../data/atlas");

	while (!glfwWindowShouldClose(window)) {

		_update_fps_counter(window);

		// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, g_gl_width, g_gl_height);
		//update logic
		CEntityManager::Get()->Update();
		//draw Sprites
		CSpriteManager::Get()->Draw();
		//draw Text
		txtrndr.DrawString("Acest joc este un test ", -1, 1.0, 50.0f);
		// update other events like input handling 
		glfwPollEvents();
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
		}
	}
	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}