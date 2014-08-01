#define _CRTDBG_MAP_ALLOC
#define _CRT_SECURE_NO_DEPRECATE
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
//#include <glm.hpp>
//#include <matrix_transform.hpp>
//#include <type_ptr.hpp>

#include "stb_image.h"
#include "stb_image.cpp"

#include <stdlib.h>
#include <crtdbg.h>

#include <stdio.h>
#include <vector>
#include <time.h>
// audio
#include <Windows.h>
#include <mmsystem.h>
using namespace std;

GLuint shader_programme;
int tex_loc;
int NMAX = 1000;
GLuint Images[20];
GLFWwindow* window;

char * LoadFileInMemory(const char *filename); // load file to buffer
void _update_fps_counter(GLFWwindow* window);
void LoadImages();
void LoadImg(char *filename, int nr);

// enenmy ship
class Enemy {
public:
	float v[20]; // vertex_buffer

	Enemy() {};
	// ordinea de desenare: 0 1 2 2 3 0
	Enemy(float x, float y, float sz) { // centrul de referinta pentru patrat si marimea
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
	~Enemy() {}
};
//XML small-texture
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
//Player ship
class Player {
public:
	float mw, mh, time, state_frame;
	float v[20];
	int curr_tex;	// current mini-texture
	int state;		// 1 idle/ 2 left/ 3 right
	int level;

	Loaded_Texture **texes;
	Player() {
		// loading texture
		texes = new Loaded_Texture*[48];
		//--- Gathering Texture info
		FILE *f = fopen("../AnimatiiExemplu/player.xml", "r");
		if (f == NULL) {
			printf("Error opening Player XML");
		}
		char *fileContent = (char*)malloc(5000);
		memset(fileContent, '\000', 5000);
		int nr = 0;
		fscanf(f, "%c", &fileContent[0]);
		while (fileContent[nr] != '\000') fscanf(f, "%c", &fileContent[++nr]);
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

		changeVectors(texes[0]);

		mh = 512; // texture height
		mw = 1024; // texture width
		level = 0;
	}
	// initializare pozitie
	void init(int lvl) {
		v[0] = -0.075f, v[1] = -0.7f, v[2] = 0.0f, v[3] = 0.0f, v[4] = 0.0f;	// stanga jos
		v[5] = 0.075f, v[6] = -0.7f, v[7] = 0.0f, v[8] = 1.0f, v[9] = 0.0f;	// dreapta jos
		v[10] = 0.075f, v[11] = -0.4f, v[12] = 0.0f, v[13] = 1.0f, v[14] = 1.0f;	// dreapta sus
		v[15] = -0.075f, v[16] = -0.4f, v[17] = 0.0f, v[18] = 0.0f, v[19] = 1.0f;	// stanga sus
		time = glfwGetTime();
		curr_tex = 0;
		changeVectors(texes[curr_tex]);
		level = lvl;
	}

	void changeVectors(Loaded_Texture *t) {
		//	printf("%d %d %d %d\n", t->x, t->y, t->width, t->height);
		v[3] = (float)t->x / mw + (float)t->width / mw, v[4] = (float)t->y / mh + (float)t->height / mh;
		v[8] = (float)t->x / mw, v[9] = (float)t->y / mh + (float)t->height / mh;
		v[13] = (float)t->x / mw, v[14] = (float)t->y / mh;
		v[18] = (float)t->x / mw + (float)t->width / mw, v[19] = (float)t->y / mh;
		//	printf("%f %f %f %f %f %f %f %f\n", v[3], v[4], v[8], v[9], v[13], v[14], v[18], v[19]);
	}

	void Update() {
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) {
			moveright();
			if (state != 1) {
				state = 1;
				curr_tex = state_frame = 0;
			}
		}
		else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) {
			moveleft();
			if (state != 2) {
				state = 2;
				curr_tex = state_frame = 0;
			}
		}
		else state = 0;
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W))
			moveup();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S))
			movedown();
		if (glfwGetTime() - time > 0.07) {
			time = glfwGetTime();
			++curr_tex;	// nr - numarul texturi folosite pentru urmatoarele frame-uri
			curr_tex %= 16;
			if (state == 0) changeVectors(texes[curr_tex]);
			if (state == 1) changeVectors(texes[curr_tex + 16]);
			if (state == 2) changeVectors(texes[curr_tex + 32]);
		}
	}

	void moveup() {
		if ((level != 1 && v[11] < 1) || v[11] < 0) {
			v[1] += 0.014f;
			v[6] += 0.014f;
			v[11] += 0.014f;
			v[16] += 0.014f;
		}
	}
	void movedown() {
		if (v[1] > -1) {
			v[1] -= 0.014f;
			v[6] -= 0.014f;
			v[11] -= 0.014f;
			v[16] -= 0.014f;
		}
	}
	void moveleft() {
		if (v[0] > -1) {
			v[0] -= 0.018f;
			v[5] -= 0.018f;
			v[10] -= 0.018f;
			v[15] -= 0.018f;
		}
	}
	void moveright() {
		if (v[10] < 1) {
			v[0] += 0.018f;
			v[5] += 0.018f;
			v[10] += 0.018f;
			v[15] += 0.018f;
		}
	}

	~Player() {
		for (int i = 0; i < 48; i++) {
			delete texes[i];
		}
		delete[] texes;
	}
};

class AnimationManager {
public:
	Enemy** enemies;	// enemy master vector
	
	Player *player;
	int nrSprites;
	
	unsigned int *index_buffer;
	GLuint elementbuffer;
	float* vertex_buffer;
	GLuint vbo;

	int flag; // 0, 1, 2, 3 (waiting screen, Lv 1, 2..)
	int bg_id, dir; // background id, intre 1,2,3 sau 4
	float *bg_buf, bg_timer;

	AnimationManager() {
		enemies = (Enemy**)malloc(1000 * sizeof(Enemy*));
		nrSprites = 0;
		index_buffer = new unsigned int[NMAX * 6];
		for (int i = 0; i < NMAX; i++) {
			index_buffer[6 * i] = 4 * i;
			index_buffer[6 * i + 1] = 4 * i + 1;
			index_buffer[6 * i + 2] = 4 * i + 2;
			index_buffer[6 * i + 3] = 4 * i + 2;
			index_buffer[6 * i + 4] = 4 * i + 3;
			index_buffer[6 * i + 5] = 4 * i;
		}
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, NMAX * 6 * sizeof(unsigned int), index_buffer, GL_DYNAMIC_DRAW);

		vertex_buffer = new float[20];
		bg_buf = new float[40];
		flag = 0;
		bg_id = 1;
		bg_init();
	}

	void addAnimation(Enemy *s) {
		enemies[nrSprites++] = s;
	}

	void addPlayer(Player *p) {
		player = p;
	}

	void allup() {
		for (int i = 0; i < nrSprites;) {
			enemies[i]->up();
			if (enemies[i]->v[1] > 1) {
				delete enemies[i];
				enemies[i] = enemies[--nrSprites];
				enemies[nrSprites] = NULL;
			}
			else {
				++i;
			}
		}
	}

	void alldown() {
		for (int i = 0; i < nrSprites;) {
			enemies[i]->down();
			if (enemies[i]->v[1] < 0) {
				delete enemies[i];
				enemies[i] = enemies[--nrSprites];
				enemies[nrSprites] = NULL;
			}
			else {
				++i;
			}
		}
	}

	void Update() {

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_0)) {
			flag = 0;
			bg_id = 1;
			bg_init();
			player->init(0);
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_1)) {
			flag = 1;
			bg_id = 2;
			bg_init();
			player->init(1);
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_2)) {
			flag = 2;
			bg_id = 3;
			bg_init();
			player->init(2);
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_3)) {
			flag = 3;
			bg_id = 4;
			bg_init();
			player->init(3);
		}

		if (flag == 0) {
		}
		if (flag == 1) {
			player->Update();
			memcpy(vertex_buffer, player->v, 20 * sizeof(float));
		}
		if (flag == 2) {
			player->Update();
			memcpy(vertex_buffer, player->v, 20 * sizeof(float));
		}
		if (flag == 3) {
			player->Update();
			memcpy(vertex_buffer, player->v, 20 * sizeof(float));
		}

		bg_update();

		// Generam un buffer in memoria video si scriem in el punctele din ram
		vbo = 0;
		glGenBuffers(1, &vbo); // generam un buffer 
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // setam bufferul generat ca bufferul curent 
		glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vertex_buffer, GL_DYNAMIC_DRAW);

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
	}

	void Draw() {
		// background
		int bg_nr = 1;
		if (bg_id == 2) bg_nr = 2;
		glBufferData(GL_ARRAY_BUFFER, bg_nr * 20 * sizeof(float), bg_buf, GL_DYNAMIC_DRAW);
		glUniform1i(tex_loc, bg_id); // use active texture 0
		glDrawElements(GL_TRIANGLES, bg_nr * 6, GL_UNSIGNED_INT, (void*)0);

		// player
		if (flag != 0) {
			glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vertex_buffer, GL_DYNAMIC_DRAW);
			glUniform1i(tex_loc, 0); // use active texture 0
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		}
	}

	void bg_init() {
		bg_timer = glfwGetTime();
		if (bg_id == 1) {
			bg_buf[0] = -1, bg_buf[1] = -1, bg_buf[2] = 0.0f, bg_buf[3] = 0.3f, bg_buf[4] = 0.3f;	// stanga jos
			bg_buf[5] = 1, bg_buf[6] = -1, bg_buf[7] = 0.0f, bg_buf[8] = 0.6f, bg_buf[9] = 0.3f;	// dreapta jos
			bg_buf[10] = 1, bg_buf[11] = 1, bg_buf[12] = 0.0f, bg_buf[13] = 0.6f, bg_buf[14] = 0.6f;	// dreapta sus
			bg_buf[15] = -1, bg_buf[16] = 1, bg_buf[17] = 0.0f, bg_buf[18] = 0.3f, bg_buf[19] = 0.6f;	// stanga sus
			dir = rand() % 4;
		}
		if (bg_id == 2) {
			bg_buf[0] = -1, bg_buf[1] = -1, bg_buf[2] = 0.0f, bg_buf[3] = 0.0f, bg_buf[4] = 0.0f;	// stanga jos
			bg_buf[5] = 1, bg_buf[6] = -1, bg_buf[7] = 0.0f, bg_buf[8] = 1.0f, bg_buf[9] = 0.0f;	// dreapta jos
			bg_buf[10] = 1, bg_buf[11] = 2, bg_buf[12] = 0.0f, bg_buf[13] = 1.0f, bg_buf[14] = 0.5f;	// dreapta sus
			bg_buf[15] = -1, bg_buf[16] = 2, bg_buf[17] = 0.0f, bg_buf[18] = 0.0f, bg_buf[19] = 0.5f;	// stanga sus
			bg_buf[20] = -1, bg_buf[21] = 2, bg_buf[22] = 0.0f, bg_buf[23] = 0.0f, bg_buf[24] = 0.5f;	// stanga jos
			bg_buf[25] = 1, bg_buf[26] = 2, bg_buf[27] = 0.0f, bg_buf[28] = 1.0f, bg_buf[29] = 0.5f;	// dreapta jos
			bg_buf[30] = 1, bg_buf[31] = 5, bg_buf[32] = 0.0f, bg_buf[33] = 1.0f, bg_buf[34] = 1.0f;	// dreapta sus
			bg_buf[35] = -1, bg_buf[36] = 5, bg_buf[37] = 0.0f, bg_buf[38] = 0.0f, bg_buf[39] = 1.0f;	// stanga sus
		}
		if (bg_id == 3 || bg_id == 4) {
			bg_buf[0] = -1, bg_buf[1] = -1, bg_buf[2] = 0.0f, bg_buf[3] = 0.0f, bg_buf[4] = 0.0f;	// stanga jos
			bg_buf[5] = 1, bg_buf[6] = -1, bg_buf[7] = 0.0f, bg_buf[8] = 1.0f, bg_buf[9] = 0.0f;	// dreapta jos
			bg_buf[10] = 1, bg_buf[11] = 1, bg_buf[12] = 0.0f, bg_buf[13] = 1.0f, bg_buf[14] = 1.0f;	// dreapta sus
			bg_buf[15] = -1, bg_buf[16] = 1, bg_buf[17] = 0.0f, bg_buf[18] = 0.0f, bg_buf[19] = 1.0f;	// stanga sus
		}
	}
	
	void bg_update() {
		if (bg_id == 1) {
			if (glfwGetTime() - bg_timer > 1.5f) {
				dir = rand() % 4;
				bg_timer = glfwGetTime();
			}
			float ct = 0.001;
			if (dir == 0) {
				if (bg_buf[3] > 0) { // left
					bg_buf[3] -= ct; bg_buf[8] -= ct; bg_buf[13] -= ct; bg_buf[18] -= ct;
				}
				else dir = 1;
			}
			if (dir == 1) {
				if (bg_buf[13] < 1) { // right
					bg_buf[3] += ct; bg_buf[8] += ct; bg_buf[13] += ct; bg_buf[18] += ct;
				}
				else dir = 0;
			}
			if (dir == 2) {
				if (bg_buf[14] < 1) { // up
					bg_buf[4] += ct; bg_buf[9] += ct; bg_buf[14] += ct; bg_buf[19] += ct;
				}
				else dir = 3;
			}
			if (dir == 3) {
				if (bg_buf[4] > 0) { // down
					bg_buf[4] -= ct; bg_buf[9] -= ct; bg_buf[14] -= ct; bg_buf[19] -= ct;
				}
				else dir = 2;
			}
		}	
		if (bg_id == 2) {
			float ct = 0.005;
			bg_buf[1] -= ct; bg_buf[6] -= ct; bg_buf[11] -= ct; bg_buf[16] -= ct;
			bg_buf[21] -= ct; bg_buf[26] -= ct; bg_buf[31] -= ct; bg_buf[36] -= ct;
			if (bg_buf[1]  <= -4) {
				bg_buf[1] = 2; bg_buf[6] = 2; bg_buf[11] = 5; bg_buf[16] = 5;
			}
			if (bg_buf[21] <= -4) {
				bg_buf[21] = 2; bg_buf[26] = 2; bg_buf[31] = 5; bg_buf[36] = 5;
			}
		}
	}

	~AnimationManager() {
		for (int i = 0; i < nrSprites; i++)
			delete enemies[i];
		free(enemies);
		delete player;
		delete[] index_buffer;
		glDeleteBuffers(1, &elementbuffer);
		delete[] vertex_buffer;
		glDeleteBuffers(1, &vbo);
		delete[] bg_buf;
	}

};

int main() {
	// Initializare (se creeaza contextul)
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	// Se creeaza fereastra
	window = glfwCreateWindow(1024, 768, "O", NULL, NULL);
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
	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	delete[] vertex_shader;
	delete[] fragment_shader;

	LoadImages();

	tex_loc = glGetUniformLocation(shader_programme, "basic_texture");
	// transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	AnimationManager *s = new AnimationManager();

	double first_press_time = glfwGetTime();
	double second_press_time;

	srand(time(NULL));

	s->addPlayer(new Player());
	//	PlaySound(TEXT("03.wav"), NULL, SND_ASYNC);

	while (!glfwWindowShouldClose(window)) {
		//..... Randare................. 
		float time1 = glfwGetTime();
		// FPS counter
		_update_fps_counter(window);
		//----------

		s->Update();
		s->Draw();

		// facem swap la buffere (Double buffer)
		glfwSwapBuffers(window);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		while (glfwGetTime() - time1 < 1.0/60); //60fps
	}

	glDetachShader(shader_programme, vs);
	glDetachShader(shader_programme, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

//	glDeleteTextures(1, &player_tex);
	glDeleteBuffers(1, &vs);
	glDeleteBuffers(1, &fs);

	delete s;
	glfwTerminate();
	_CrtDumpMemoryLeaks();
	//	PlaySound(TEXT("04.wav"), NULL, SND_SYNC);

	return 0;
}

// loading images
void LoadImages() {
	for (int i = 0; i < 10; i++)
		Images[i] = i;
	LoadImg("../AnimatiiExemplu/player.png", 0);
	LoadImg("../Data/Sprites/bg0.png", 1);
	LoadImg("../Data/Sprites/bg1.png", 2);
	LoadImg("../Data/Sprites/bg2.png", 3);
	LoadImg("../Data/Sprites/bg3.png", 4);
//	LoadImg();
}

// load image
void LoadImg(char* filename, int nr) {
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load(filename, &x, &y, &n, force_channels);
	// Trimitem textura la memoria video
	glGenTextures(1, &Images[nr]);
	glActiveTexture(GL_TEXTURE0 + nr);
	glBindTexture(GL_TEXTURE_2D, Images[nr]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	// setam parametri de sampling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //ce se intampla cand coordonata nu se inscrie in limite
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //ce se intampla cand coordonata nu se inscrie in limite
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // setam samplare cu interpolare liniara
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // setam samplare cu interpolare liniara
	stbi_image_free(image_data);
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
