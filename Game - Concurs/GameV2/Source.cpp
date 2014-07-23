#define _CRTDBG_MAP_ALLOC
#define _CRT_SECURE_NO_DEPRECATE
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <glm.hpp>
#include <matrix_transform.hpp>
#include <type_ptr.hpp>

#include "stb_image.h"
#include "stb_image.cpp"

#include <stdlib.h>
#include <crtdbg.h>

#include <stdio.h>
#include <vector>
#include <time.h>
using namespace std;

char * LoadFileInMemory(const char *filename); // load file to buffer
void _update_fps_counter(GLFWwindow* window, int nrp);

static unsigned int next_color = 0;
const float step = 1.0f / 4096;
const int NMAX = 1000000;
// patrat, 4 noduri
class Sprite {
public:
	float v[20]; // vertex_buffer
	int direction;
	Sprite() {};
	// ordinea de desenare: 0 1 2 2 3 0
	Sprite(float x, float y, float sz) { // centrul de referinta pentru patrat si marimea
		v[0] = -sz + x, v[1] = -sz + y, v[2] = 0.0f, v[3] = 1.0f, v[4] = 1.0f;		// stanga jos
		v[5] = sz + x, v[6] = -sz + y, v[7] = 0.0f, v[8] = 0.0f, v[9] = 1.0f;		// dreapta jos
		v[10] = sz + x, v[11] = sz + y, v[12] = 0.0f, v[13] = 0.0f, v[14] = 0.0f;	// dreapta sus
		v[15] = -sz + x, v[16] = sz + y, v[17] = 0.0f, v[18] = 1.0f, v[19] = 0.0f;	// stanga sus
		direction = 0;
		spriteColor();
	}

	void spriteColor() {
		// uniform
//		float row = (float)(next_color / 4096) / 4096;
//		float col = (float)(next_color % 4096) / 4096;
		// random
		float row = (float)(rand() % 4096) / 4096;
		float col = (float)(rand() % 4096) / 4096;
		v[3] = col + step, v[4] = row + step;	// stanga jos
		v[8] = col, v[9] = row + step;	// dreapta jos
		v[13] = col, v[14] = row;	// dreapta sus
		v[18] = col + step, v[19] = row;	// stanga sus
		++next_color;
		if (next_color > 4096 * 4096) next_color = 0;
	}

	void move() {
		if (direction == 0) {// dreapta
			v[0] += 0.005, v[5] += 0.005;
			v[10] += 0.005, v[15] += 0.005;
			if (v[5] >= 1)
				direction = 1;
		}
		else {// stanga
			v[0] -= 0.005, v[5] -= 0.005;
			v[10] -= 0.005, v[15] -= 0.005;
			if (v[0] <= -1)
				direction = 0;
		}
	}
	~Sprite() {}
};

class SpriteManager {
public:
	Sprite** sprites;	// sprite master vector
	int nrSprites;
	unsigned int* index_buffer;
	float* vertex_buffer;
	SpriteManager() {
		sprites = (Sprite**)malloc(NMAX * sizeof(Sprite*));
		nrSprites = 0;
		index_buffer = (unsigned int*)malloc(NMAX * 6 * sizeof(unsigned int));
		vertex_buffer = (float*)malloc(NMAX * 20 * sizeof(float));
		for (int i = 0; i < NMAX; i++) {
			index_buffer[6 * i] = 4 * i;
			index_buffer[6 * i + 1] = 4 * i + 1;
			index_buffer[6 * i + 2] = 4 * i + 2;
			index_buffer[6 * i + 3] = 4 * i + 2;
			index_buffer[6 * i + 4] = 4 * i + 3;
			index_buffer[6 * i + 5] = 4 * i;
		}
	}

	void addSprite() {
		float obj_size = 0.0005;
		sprites[nrSprites] = new Sprite((rand() % 2002 - 1000) * (2 - 2 * obj_size) / 2000,
			(rand() % 2002 - 1000) * (2 - 2 * obj_size) / 2000, obj_size);
		memcpy(vertex_buffer + 20 * nrSprites, sprites[nrSprites]->v, 20 * sizeof(float));
		nrSprites++;
	}

	void move() {
		for (int i = 0; i < nrSprites; i++) {
			sprites[i]->move();
			memcpy(vertex_buffer + 20 * i, sprites[i]->v, 20 * sizeof(float));
		}
	}

	void deleteSprite() {
		delete sprites[--nrSprites];
		sprites[nrSprites] = NULL;
	}

	void Update(int flag) {
		move();
		int nr_obj = 1000;
		if (flag == 1) { // adaug nr_obj
			for (int i = 0; i < nr_obj; i++) {
				addSprite();
			}
		}
		if (flag == 0) {// sterg nr_obj
			for (int i = 0; i < nr_obj; i++) {
				if (nrSprites > 0) {
					deleteSprite();
				}
			}
		}
	}

	void Draw() {
		GLuint elementbuffer;
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nrSprites * 6 * sizeof(unsigned int), index_buffer, GL_STATIC_DRAW);

		// Draw the triangles !
		glDrawElements(GL_TRIANGLES, 6 * nrSprites, GL_UNSIGNED_INT, (void*)0);
		glDeleteBuffers(1, &elementbuffer);
	}
	~SpriteManager() {
		for (int i = 0; i < nrSprites; i++)
			delete sprites[i];
		free(sprites);
		free(index_buffer);
		free(vertex_buffer);
	}

};

int main() {
	// Initializare (se creeaza contextul)
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	// Se creeaza fereastra
	GLFWwindow* window = glfwCreateWindow(1024, 768, "Workshop2", NULL, NULL);
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

	const char * vertex_shader = LoadFileInMemory("../Data/vertexShader.glsl");
	const char * fragment_shader = LoadFileInMemory("../Data/fragmentShader.glsl");

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

	// incarcam imaginea din fisier si ii fortam canalele RGBA
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load("../Data/Sprites/RBG.png", &x, &y, &n, force_channels);

	// Trimitem textura la memoria video
	GLuint tex = 0;
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	// setam parametri de sampling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //ce se intampla cand coordonata nu se inscrie in limite
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //ce se intampla cand coordonata nu se inscrie in limite
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // setam samplare cu interpolare liniara
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // setam samplare cu interpolare liniara

	int tex_loc = glGetUniformLocation(shader_programme, "basic_texture");
	// transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand(time(NULL));
	SpriteManager *s = new SpriteManager();
	int flag = 1, stabil = 0;
	float t1, stab_time = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		//..... Randare................. 
		float time1 = glfwGetTime();
		// FPS counter
		_update_fps_counter(window, s->nrSprites);
		//----------

		// Generam un buffer in memoria video si scriem in el punctele din ram
		GLuint vbo = 0;
		glGenBuffers(1, &vbo); // generam un buffer 
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // setam bufferul generat ca bufferul curent 
		glBufferData(GL_ARRAY_BUFFER, s->nrSprites * 20 * sizeof(float), s->vertex_buffer, GL_STATIC_DRAW);

		// Specify the layout of the vertex data
		GLint posAttrib = glGetAttribLocation(shader_programme, "vertex_position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

		GLint texAttrib = glGetAttribLocation(shader_programme, "texture_coordinates");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		//----------
		// stergem ce s-a desenat anterior
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// spunem ce shader vom folosi pentru desenare
		glUseProgram(shader_programme);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(tex_loc, 0); // use active texture 0
		float time2 = glfwGetTime();
		// timing 60 fps		
		s->Update(flag);
		// sleep
		t1 = glfwGetTime() - time2 + time1;
		s->Draw();
		glDeleteBuffers(1, &vbo);

		// facem swap la buffere (Double buffer)
		glfwSwapBuffers(window);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}if (glfwGetTime() - stab_time > 10) {
			stabil = 1;
		}
		if (!stabil) {
			if (glfwGetTime() - t1 > 0.009) {
				flag = 0;
			}
			else {
				flag = 1;
				while (glfwGetTime() - t1 < 0.016);
			}
		}
		else {
			flag = 2;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				s->addSprite();
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				s->deleteSprite();
			}
		}
	}

	glDetachShader(shader_programme, vs);
	glDetachShader(shader_programme, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	glDeleteTextures(1, &tex);
	glDeleteBuffers(1, &vs);
	glDeleteBuffers(1, &fs);

	delete s;
	stbi_image_free(image_data);

	glfwTerminate();
	_CrtDumpMemoryLeaks();

	return 0;
}

// fps_counter
void _update_fps_counter(GLFWwindow* window, int nrp) {
	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	if (elapsed_seconds > 0.25) {
		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];
		sprintf(tmp, "fps: %.2f, particles: %d", fps, nrp);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count++;
}

// load file in a buffer
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
	buffer[size] = '\000';
	return buffer;
}
