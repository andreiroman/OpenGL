#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// functie banala de incarcat continutul unui fisier intr-un buffer
char * LoadFileInMemory(const char *filename)
{
	int size = 0;
	char *buffer = NULL;
	FILE *f = fopen(filename, "rb");
	if (f == NULL)
	{
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = new char[size + 1];
	if (size != fread(buffer, sizeof(char), size, f))
	{
		delete[] buffer;
	}
	fclose(f);
	buffer[size] = 0;
	return buffer;
}


int main() {

	// logging
	/*
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLDebugCallback, NULL);
	*/
	// Initializare (se creeaza contextul)
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	// Se creeaza fereastra
	GLFWwindow* window = glfwCreateWindow(640, 480, "Workshop1", NULL, NULL);
	if (!window) {
		// nu am reusit sa facem fereastra, oprim totul si dam mesaj de eroare
		printf("ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	// Atasam contextul de fereastra
	glfwMakeContextCurrent(window);

	// Pornit extension handler-ul
	glewInit();

	// Vedem versiunile
	const GLubyte* renderer = glGetString(GL_RENDERER); //renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	const char * vertex_shader = LoadFileInMemory("../data/vertexShader.glsl");
	const char * fragment_shader = LoadFileInMemory("../data/fragmentShader.glsl");

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	GLuint shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	delete[] vertex_shader;
	delete[] fragment_shader;

	// buffer cu vertecsi in RAM 
	float vertex_buffer[] = {
		0.25f, -0.25f, 0.0f,
		-0.25f, -0.25f, 0.0f,
		0.25f, 0.25f, 0.0f,
		- 0.25f, 0.25f, 0.0f
	};
	
	// touple (R, G, B)
	float g_color_buffer_data[] = {
		0.0f, 0.2f, 0.2f,
		0.0f, 0.4f, 0.4f,
		0.0f, 0.6f, 0.6f,
		0.0f, 0.8f, 0.8f
	};
	
	// Generam un buffer in memoria video si scriem in el culorile din ram
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	// slot pentru atributul 1
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	float key = 0.001;
	while (!glfwWindowShouldClose(window)) {
		//..... Randare................. 
		// Generam un buffer in memoria video si scriem in el punctele din ram
		GLuint vbo = 0;
		glGenBuffers(1, &vbo); // generam un buffer 
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // setam bufferul generat ca bufferul curent 
		glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), vertex_buffer, GL_STATIC_DRAW); //  scriem in bufferul din memoria video informatia din bufferul din memoria RAM

		// De partea aceasta am uitat sa va spun la curs -> Pentru a defini bufferul alocat de opengl ca fiind buffer de in de atribute, stream de vertecsi trebuie sa :
		// 1. Ii spunem OpenGL-ului ca vom avea un slot pentru acest atribut (in cazul nostru 0) , daca mai aveam vreun atribut ar fi trebuit si acela enablat pe alt slot (de exemplu 1)
		// 2. Definit bufferul ca Vertex Attribute Pointer cu glVertexAttribPointer
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		// deplasare
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && vertex_buffer[10] < 1) {
			vertex_buffer[1] += key;
			vertex_buffer[4] += key;
			vertex_buffer[7] += key;
			vertex_buffer[10] += key;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && vertex_buffer[3] > -1) {
			vertex_buffer[0] -= key;
			vertex_buffer[3] -= key;
			vertex_buffer[6] -= key;
			vertex_buffer[9] -= key;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && vertex_buffer[1] > -1) {
			vertex_buffer[1] -= key;
			vertex_buffer[4] -= key;
			vertex_buffer[7] -= key;
			vertex_buffer[10] -= key;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && vertex_buffer[0] < 1) {
			vertex_buffer[0] += key;
			vertex_buffer[3] += key;
			vertex_buffer[6] += key;
			vertex_buffer[9] += key;
		}
		
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) break;

		// stergem ce s-a desenat anterior
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// spunem ce shader vom folosi pentru desenare
		glUseProgram(shader_programme);
		// facem bind la vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// draw points 0-3 from the currently bound VAO with current in-use shader
		// glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 4);

		// facem swap la buffere (Double buffer)
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glDeleteBuffers(12, &colorbuffer);
	glfwTerminate();

	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();

	return 0;
}