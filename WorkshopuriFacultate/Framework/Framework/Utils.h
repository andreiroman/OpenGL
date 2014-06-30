#ifndef __H_UTILS_
#define __H_UTILS_

#include <time.h>
#include <stdarg.h>

#define GL_LOG_FILE "gl.log"


char * LoadFileInMemory(const char *filename);
bool RestartLog();
bool GameLog(const char* message, ...);
bool ErrorLog(const char* message, ...);
void LogOpenGLParams();

void FlipTexture(unsigned char* image_data, int x, int y, int n);
int LoadTexture(const char* filename, int &width, int &height);



#endif