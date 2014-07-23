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
void _update_fps_counter(GLFWwindow* window);

// patrat, 4 noduri
class Sprite {
public:
	float v[20]; // vertex_buffer

	Sprite() {};
	// ordinea de desenare: 0 1 2 2 3 0
	Sprite(float x, float y, float sz) { // centrul de referinta pentru patrat si marimea
		v[0] = -sz + x, v[1] = -sz + y, v[2] = 0.0f, v[3] = 1.0f, v[4] = 1.0f;		// stanga jos
		v[5] = sz + x, v[6] = -sz + y, v[7] = 0.0f, v[8] = 0.0f, v[9] = 1.0f;		// dreapta jos
		v[10] = sz + x, v[11] = sz + y, v[12] = 0.0f, v[13] = 0.0f, v[14] = 0.0f;	// dreapta sus
		v[15] = -sz + x, v[16] = sz + y, v[17] = 0.0f, v[18] = 1.0f, v[19] = 0.0f;	// stanga sus
	}
	void up() {
		v[1] += 0.0005, v[6] += 0.0005;
		v[11] += 0.0005, v[16] += 0.0005;
	}
	void down() {
		v[1] -= 0.0005, v[6] -= 0.0005;
		v[11] -= 0.0005, v[16] -= 0.0005;
	}
	~Sprite() {}
};

class Loaded_Texture {
public:
	int id, width, height, x, y;
	char* imageName;

	Loaded_Texture() {
		imageName = (char*)calloc(15, 1);
	}

	void set_texture_id(int texture) {
		id = texture;
	}
	void setx(int a) {
		x = a;
	}
	void sety(int a) {
		y = a;
	}

	void set_texture_name(char *c) {
		memcpy(imageName, c, 15);
	}

	void set_height(int h) {
		height = h;
	}

	void set_width(int w) {
		width = w;
	}

	~Loaded_Texture() {
		free(imageName);
	}
};

class Player {
public:
	float mw, mh, speed;
	float v[20];
	Player() {
		v[0] = -0.25f,	v[1] = -0.25f,	v[2] = 0.0f,	v[3] = 0.0f,	v[4] = 0.0f;	// stanga jos
		v[5] = 0.25f,	v[6] = -0.25f,	v[7] = 0.0f,	v[8] = 1.0f,	v[9] = 0.0f;	// dreapta jos
		v[10] = 0.25f,	v[11] = 0.25f,	v[12] = 0.0f,	v[13] = 1.0f,	v[14] = 1.0f;	// dreapta sus
		v[15] = -0.25f,	v[16] = 0.25f,	v[17] = 0.0f,	v[18] = 0.0f,	v[19] = 1.0f;	// stanga sus
		mh = 512; // texture height
		mw = 1024; // texture width
		speed = 0.01;
	}

	void changeVectors(Loaded_Texture *t) {
	//	printf("%d %d %d %d\n", t->x, t->y, t->width, t->height);
		v[3] = (float)t->x / mw + (float)t->width / mw, v[4] = (float)t->y / mh + (float)t->height / mh;
		v[8] = (float)t->x / mw, v[9] = (float)t->y / mh + (float)t->height / mh;
		v[13] = (float)t->x / mw, v[14] = (float)t->y / mh;
		v[18] = (float)t->x / mw + (float)t->width / mw, v[19] = (float)t->y / mh;
		printf("%f %f %f %f %f %f %f %f\n",
			v[3], v[4], v[8], v[9], v[13], v[14], v[18], v[19]);
	}

	void moveup() {
		if (v[11] < 1) {
			v[1] += 0.0001f;
			v[6] += 0.0001f;
			v[11] += 0.0001f;
			v[16] += 0.0001f;
		}
	}
	void movedown() {
		if (v[1] > -1) {
			v[1] -= 0.0001f;
			v[6] -= 0.0001f;
			v[11] -= 0.0001f;
			v[16] -= 0.0001f;
		}
	}
	void moveleft() {
		if (v[0] > -1) {
			v[0] -= 0.0001f;
			v[5] -= 0.0001f;
			v[10] -= 0.0001f;
			v[15] -= 0.0001f;
		}
	}
	void moveright() {
		if (v[10] < 1) {
			v[0] += 0.0001f;
			v[5] += 0.0001f;
			v[10] += 0.0001f;
			v[15] += 0.0001f;
		}
	}

	~Player() {}
};

class SpriteManager {
public:
	Sprite** sprites;	// sprite master vector
	Player *player;
	int nrSprites;
	SpriteManager() {
		sprites = (Sprite**)malloc(1000 * sizeof(Sprite*));
		nrSprites = 0;
	}

	void addSprite(Sprite *s) {
		sprites[nrSprites++] = s;
	}

	void setPlayer(Player *player1) {
		player = player1;
	}

	void allup() {
		for (int i = 0; i < nrSprites;) {
			sprites[i]->up();
			if (sprites[i]->v[1] > 1) {
				delete sprites[i];
				sprites[i] = sprites[--nrSprites];
				sprites[nrSprites] = NULL;
			}
			else {
				++i;
			}
		}
	}

	void alldown() {
		for (int i = 0; i < nrSprites;) {
			sprites[i]->down();
			if (sprites[i]->v[1] < 0) {
				delete sprites[i];
				sprites[i] = sprites[--nrSprites];
				sprites[nrSprites] = NULL;
			}
			else {
				++i;
			}
		}
	}

	void Draw() {
		unsigned int *index_buffer = new unsigned int[6];
		nrSprites = 1;
		index_buffer[0] = 0;
		index_buffer[1] = 1;
		index_buffer[2] = 2;
		index_buffer[3] = 2;
		index_buffer[4] = 3;
		index_buffer[5] = 0;

		GLuint elementbuffer;
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nrSprites * 6 * sizeof(unsigned int), index_buffer, GL_STATIC_DRAW);

		// Draw the triangles !
		glDrawElements(GL_TRIANGLES, 6 * nrSprites, GL_UNSIGNED_INT, (void*)0);
		delete[] index_buffer;
		glDeleteBuffers(nrSprites * 6 * sizeof(unsigned int), &elementbuffer);
		nrSprites = 0;
	}
	~SpriteManager() {
		for (int i = 0; i < nrSprites; i++)
			delete sprites[i];
		free(sprites);
		delete player;
	}

};

int main() {
	// Initializare (se creeaza contextul)
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	// Se creeaza fereastra
	GLFWwindow* window = glfwCreateWindow(640, 480, "Workshop2", NULL, NULL);
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
	unsigned char* image_data = stbi_load("../AnimatiiExemplu/player.png", &x, &y, &n, force_channels);

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

	SpriteManager *s = new SpriteManager();

	double first_press_time = glfwGetTime();
	double second_press_time;

	srand(time(NULL));
	Player *player = new Player();

	Loaded_Texture **texes = new Loaded_Texture*[48];
	//--- Gathering Texture info
	FILE *f = fopen("../AnimatiiExemplu/player.xml", "r");
	if (f == NULL) {
		printf("Error opening XML");
		return 1;
	}
	char *fileContent = (char*)malloc(5000);
	memset(fileContent, '\000', 5000);
	int nr = 0;
	fscanf(f, "%c", &fileContent[0]);
	while (fileContent[nr] != '\000') {
		fscanf(f, "%c", &fileContent[++nr]);
	}
	fclose(f); // Am citit tot continutul fisierului xml in p

	nr = 0;
	char* pch = strtok(fileContent, "\"");
	pch = strtok(NULL, "\""), pch = strtok(NULL, "\""), pch = strtok(NULL, "\"");
	while (pch != NULL) {
		texes[nr] = new Loaded_Texture();
		texes[nr]->set_texture_id(nr);
		texes[nr]->set_texture_name(pch);
		pch = strtok(NULL, "\""), pch = strtok(NULL, "\"");
		texes[nr]->setx(atoi(pch));
		pch = strtok(NULL, "\""), pch = strtok(NULL, "\"");
		texes[nr]->sety(atoi(pch));
		pch = strtok(NULL, "\""), pch = strtok(NULL, "\"");
		texes[nr]->set_width(atoi(pch));
		pch = strtok(NULL, "\""), pch = strtok(NULL, "\"");
		texes[nr]->set_height(atoi(pch));
		pch = strtok(NULL, "\""), pch = strtok(NULL, "\"");
		nr++;
	}
	free(fileContent);

	//---
	float animation_time = glfwGetTime();
	nr = 0;
	player->changeVectors(texes[0]);
	int state = 0; // Idle 0..15, Left 16..31, Right 32..47
	int state_frame = 0;
	float idle_time = 0;

	while (!glfwWindowShouldClose(window)) {
		//..... Randare................. 
		// FPS counter
		_update_fps_counter(window);
		//----------
		if (idle_time > 0.2) {
			state = 0;
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) {
			player->moveright();
			idle_time = 0;
			if (state != 1) {
				state = 1;
				nr = state_frame = 0;
			}
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) {
			player->moveleft();
			idle_time = 0;
			if (state != 2) {
				state = 2;
				nr = state_frame = 0;
			}
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) {
			player->moveup();
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) {
			player->movedown();
		}
		if (glfwGetTime() - animation_time > 0.05) {
			idle_time += 0.1;
			animation_time = glfwGetTime();
			++nr;	// nr - numarul texturi folosite pentru urmatoarele frame-uri
			nr %= 16;
			if (state == 0) {
				player->changeVectors(texes[nr]);
			}
			if (state == 1) {
				player->changeVectors(texes[nr + 16]);
			}
			if (state == 2) {
				player->changeVectors(texes[nr + 32]);
			}
		}

		int nrE = 1;
		float *vertex_buffer = new float[nrE * 20];
		memcpy(vertex_buffer, player->v, 20 * sizeof(float));

		// Generam un buffer in memoria video si scriem in el punctele din ram
		GLuint vbo = 0;
		glGenBuffers(1, &vbo); // generam un buffer 
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // setam bufferul generat ca bufferul curent 
		glBufferData(GL_ARRAY_BUFFER, nrE * 20 * sizeof(float), vertex_buffer, GL_STATIC_DRAW);

		// Specify the layout of the vertex data
		GLint posAttrib = glGetAttribLocation(shader_programme, "vertex_position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

		GLint texAttrib = glGetAttribLocation(shader_programme, "texture_coordinates");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		//----------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// spunem ce shader vom folosi pentru desenare
		glUseProgram(shader_programme);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(tex_loc, 0); // use active texture 0

		s->Draw();
		delete[] vertex_buffer;
		glDeleteBuffers(1, &vbo);

		// facem swap la buffere (Double buffer)
		glfwSwapBuffers(window);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
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
	delete player;
	stbi_image_free(image_data);

	for (int i = 0; i < 48; i++) {
		delete texes[i];
	}
	delete[] texes;

	glfwTerminate();
	_CrtDumpMemoryLeaks();

	return 0;
}

// fps_counter
void _update_fps_counter(GLFWwindow* window) {
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
