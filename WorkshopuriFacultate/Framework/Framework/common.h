#ifndef ___H_COMMON_H
#define ___H_COMMON_H

#include <GL/glew.h>
#include <assert.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern	int g_gl_width;
extern	int g_gl_height;
extern	GLFWwindow* window;

#define SCREEN_LEFT -2.0f
#define SCREEN_RIGHT +2.0f
#define SCREEN_TOP	 2.0f
#define SCREEN_BOTTOM -2.0f

enum VERTEX_ATRIBUTES
{
	POSITION = 0,
	TEXTURE_COORDINATE_0 = 1
};


#endif //___H_COMMON_H