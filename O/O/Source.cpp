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
#include <cmath>
#include <time.h>
// audio
#include <Windows.h>
#include <mmsystem.h>
using namespace std;

GLuint shader_programme;
int tex_loc;
int NMAX = 1000;
GLuint Images[30];
GLFWwindow* window;
int screen_width = 1024;
int screen_height = 768;

char * LoadFileInMemory(const char *filename); // load file to buffer
void _update_fps_counter(GLFWwindow* window);
void LoadImages();
void LoadImg(char *filename, int nr);
int game1_score;
int game2_score;

//XML small texture
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
// explosion
class Explosion {
public:
	float v[20];
	int type;
	int nr_tex = 0;
	float step;

	Explosion(int type, float x, float y) {
		this->type = type;
		float size;
		if (type == 1) size = 0.05;
		if (type == 2) size = 0.1;
		if (type == 3) size = 0.04;
		v[0] = x - size, v[1] = y - size, v[2] = 0.0f, v[3] = 0.0f, v[4] = 1.0f;	// stanga jos
		v[5] = x + size, v[6] = y - size, v[7] = 0.0f, v[8] = 1.0 / 40, v[9] = 1.0f;	// dreapta jos
		v[10] = x + size, v[11] = y + size, v[12] = 0.0f, v[13] = 1.0 / 40, v[14] = 0.0f;	// dreapta sus
		v[15] = x - size, v[16] = y + size, v[17] = 0.0f, v[18] = 0.0f, v[19] = 0.0f;	// stanga sus
		if (type == 1) step = 1.0 / 40;
		if (type == 2) step = 1.0 / 64;
		if (type == 3) step = 1.0 / 48;
		v[8] = v[13] = step;
	}
	void Update() {
		v[3] += step; v[13] += step;
		v[8] += step; v[18] += step;
		++nr_tex;
	}
	int isTerminated() {
		if (type == 1) return nr_tex == 40;
		if (type == 2) return nr_tex == 64;
		if (type == 3) return nr_tex == 48;
	}
	void Draw() {
		glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), v, GL_DYNAMIC_DRAW);
		if (type == 1)
			glUniform1i(tex_loc, 14);
		if (type == 2)
			glUniform1i(tex_loc, 15);
		if (type == 3)
			glUniform1i(tex_loc, 16);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	}
	~Explosion() {}
};
// resource
class Resource {
public:
	float v[20];
	int type;
	int nr_tex = 0;
	float step;
	float speed;
	float size;
	float update_time = 0.1;
	float prev_time;

	Resource() {
		if (rand() % 100 < 70) type = 1;
		else type = 2;
		size = (float)(rand() % 600 + 150) / 10000;
		speed = (float)(rand() % 200 + 50) / 10000;
		float loc = (float)(rand() % 95 - 50) / 55;
		if (type == 1) step = 1.0 / 12;
		if (type == 2) step = 1.0 / 8;
		v[0] = loc - size, v[1] = 1.0f, v[2] = 0.0f, v[3] = 0.0f, v[4] = 1.0f;	// stanga jos
		v[5] = loc + size, v[6] = 1.0f, v[7] = 0.0f, v[8] = step, v[9] = 1.0f;	// dreapta jos
		v[10] = loc + size, v[11] = 1.05f + size * 5, v[12] = 0.0f, v[13] = step, v[14] = 0.0f;	// dreapta sus
		v[15] = loc - size, v[16] = 1.05f + size * 5, v[17] = 0.0f, v[18] = 0.0f, v[19] = 0.0f;	// stanga sus
		prev_time = glfwGetTime();
	}
	void Update() {
		if (glfwGetTime() - prev_time > update_time) {
			prev_time = glfwGetTime();
			if (type == 1) nr_tex %= 8;
			if (type == 2) nr_tex %= 7;
			v[3] += step; v[13] += step;
			v[8] += step; v[18] += step;
			if (nr_tex == 0 && type == 1) {
				v[3] = v[18] = step;
				v[8] = v[13] = step + step;
			}
			if (nr_tex == 0 && type == 2) {
				v[3] = v[18] = 0;
				v[8] = v[13] = step;
			}
			++nr_tex;
		}
		v[1] -= speed, v[6] -= speed, v[11] -= speed, v[16] -= speed;
	}
	int Offscreen() {
		return v[11] < -1;
	}
	void Draw() {
		glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), v, GL_DYNAMIC_DRAW);
		if (type == 1)
			glUniform1i(tex_loc, 19);
		if (type == 2)
			glUniform1i(tex_loc, 20);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	}
	~Resource() {}
};
//bullet
class Bullet {
public:
	float v[20];
	int frame;
	const float unit = 1.0 / 16;
	int id; // 1-friendly 2-enemy

	Bullet(float *ref, int id) {
		v[0] = ref[0] + 0.03, v[1] = ref[1], v[2] = 0.0f, v[3] = 0.0f, v[4] = 1.0f;	// stanga jos
		v[5] = ref[5] - 0.03, v[6] = ref[6], v[7] = 0.0f, v[8] = 1.0 / 16, v[9] = 1.0f;	// dreapta jos
		v[10] = ref[10] - 0.03, v[11] = ref[11], v[12] = 0.0f, v[13] = 1.0 / 16, v[14] = 0.0f;	// dreapta sus
		v[15] = ref[15] + 0.03, v[16] = ref[16], v[17] = 0.0f, v[18] = 0.0f, v[19] = 0.0f;	// stanga sus
		frame = 0;
		this->id = id;
		if (id == 1) {
			v[1] += 0.1; v[6] += 0.1; v[11] += 0.05; v[16] += 0.05;
		}
		if (id == 2) {
			v[11] -= 0.15; v[16] -= 0.15;
			v[4] = v[9] = 0;
			v[14] = v[19] = 1;
		}
	}

	void Update() {
		float ct = 0.03;
		if (id == 1)
			v[1] += ct, v[6] += ct, v[11] += ct, v[16] += ct;
		if (id == 2)
			v[1] -= ct / 2, v[6] -= ct / 2, v[11] -= ct / 2, v[16] -= ct / 2;
		frame %= 16;
		if (!frame) {
			v[3] = v[18] = 0;
			v[8] = v[13] = unit;
		}
		else {
			v[3] += unit, v[13] += unit;
			v[8] += unit, v[18] += unit;
		}
		++frame;
	}

	bool Offscreen() {
		if (v[11] > 1 && id == 1) return true;
		if (v[11] < -1 && id == 2) return true;
		return false;
	}

	void Draw() {
		glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), v, GL_DYNAMIC_DRAW);
		if (id == 1)
			glUniform1i(tex_loc, 11);
		if (id == 2)
			glUniform1i(tex_loc, 12);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	}

	~Bullet() {}
};
// enenmy ship
class Enemy {
public:
	float v[20]; // vertex_buffer
	const float step = 1.0 / 16;
	int nr_frame;

	Bullet **bullets;
	float last_shot;
	float bullet_delay_time;
	int nr_bullets;

	const float rewind_time = 0.1;
	float rewind; // ship back to normal position
	int rewind_state;

	int smart_enemy;
	float smart_speed;
	int smart_dir;
	float ship_speed;
	float animation_start;
	const float animation_delay = 2.0 / 60;
	int health;
	int must_shoot;

	Enemy() { // centrul de referinta pentru patrat si marimea
		float loc = (float)(rand() % 95 - 50) / 55;
		v[0] = loc - 0.075, v[1] = 1.0f, v[2] = 0.0f, v[3] = 0.0f, v[4] = 1.0f;	// stanga jos
		v[5] = loc + 0.075, v[6] = 1.0f, v[7] = 0.0f, v[8] = 1.0 / 16, v[9] = 1.0f;	// dreapta jos
		v[10] = loc + 0.075, v[11] = 1.3f, v[12] = 0.0f, v[13] = 1.0 / 16, v[14] = 0.0f;	// dreapta sus
		v[15] = loc - 0.075, v[16] = 1.3f, v[17] = 0.0f, v[18] = 0.0f, v[19] = 0.0f;	// stanga sus
		nr_frame = 0;

		bullet_delay_time = (float)(rand() % 50) / 50 + 1.5;
		bullets = (Bullet**)malloc(50 * sizeof(Bullet*));
		last_shot = glfwGetTime() - 1;
		rewind = glfwGetTime();
		rewind_state = 0;
		nr_bullets = 0;

		if(rand() % 100 < 50) smart_enemy = 1;
		else smart_enemy = 0;
		smart_speed = (float)(rand() % 50 + 50) / 10000;
		
		ship_speed = (float)(rand() % 1000 + 500) / 100000;
		animation_start = glfwGetTime();
		health = 100;
	}

	void Update(float *pv) {
		if (smart_enemy) {
			if (pv[5] + 0.002 > v[5]) smart_dir = 1;
			else smart_dir = -1;
			if (pv[5] + 0.02 > v[5] && pv[5] - 0.02 < v[5]) {
				v[0] = pv[0]; v[5] = pv[5];
				v[10] = pv[10]; v[15] = pv[15];
			}
			if (abs(pv[5] - v[5]) > 0.02) {
				v[0] += smart_speed * smart_dir, v[5] += smart_speed * smart_dir;
				v[10] += smart_speed * smart_dir, v[15] += smart_speed * smart_dir;
			}
		}
		nr_frame %= 16;

		if (glfwGetTime() - animation_start > animation_delay) {
			animation_start = glfwGetTime();
			if (nr_frame == 0) {
				v[3] = v[18] = 0;
				v[8] = v[13] = 1.0 / 16;
			}
			else {
				v[3] = v[18] += 1.0 / 16;
				v[8] = v[13] += 1.0 / 16;
			}
			++nr_frame;
		}
		v[1] -= ship_speed, v[6] -= ship_speed;
		v[11] -= ship_speed, v[16] -= ship_speed;
		// bullets
		float ct = 0.02;
		if (rewind_state && glfwGetTime() - rewind > rewind_time) {
			v[1] -= ct, v[6] -= ct;
			v[11] -= ct, v[16] -= ct;
			rewind_state = 0;
		}
		if (glfwGetTime() - last_shot > bullet_delay_time) {
			last_shot = glfwGetTime();
			must_shoot = 1;
			v[1] += ct, v[6] += ct;
			v[11] += ct, v[16] += ct;
			rewind = glfwGetTime();
			rewind_state = 1;
		}
	}
	void Draw() {
		// bullets
		for (int i = 0; i < nr_bullets; i++) {
			bullets[i]->Draw();
		}

		glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), v, GL_DYNAMIC_DRAW);
		if (smart_enemy == 0) glUniform1i(tex_loc, 13);
		if (smart_enemy == 1) glUniform1i(tex_loc, 18);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	}
	int Offscreen() {
		return v[11] < -1.0;
	}
	~Enemy() {
		for (int i = 0; i < nr_bullets; i++)
			delete bullets[i];
		delete[] bullets;
	}
};
//Player ship
class Player {
public:
	float mw, mh, time, state_frame;
	float v[20], th[20];
	int curr_tex;	// current mini-texture
	int state;		// 1 idle/ 2 left/ 3 right
	int level;
	int thrust = 0;
	int health = 1000;

	Bullet **bullets;
	float last_shot = 0;
	const float bullet_delay_time = 0.15;
	int nr_bullets;
		
	const float rewind_time = 0.1;
	float rewind; // ship back to normal position
	int rewind_state = 0;
	int can_shoot = 1;

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
		bullets = (Bullet**)malloc(100 * sizeof(Bullet*));
		rewind = glfwGetTime();
	}
	// initializare pozitie
	void init(int lvl) {
		while (nr_bullets) delete bullets[--nr_bullets];
		v[0] = -0.075f, v[1] = -0.7f, v[2] = 0.0f, v[3] = 0.0f, v[4] = 0.0f;	// stanga jos
		v[5] = 0.075f, v[6] = -0.7f, v[7] = 0.0f, v[8] = 1.0f, v[9] = 0.0f;	// dreapta jos
		v[10] = 0.075f, v[11] = -0.4f, v[12] = 0.0f, v[13] = 1.0f, v[14] = 1.0f;	// dreapta sus
		v[15] = -0.075f, v[16] = -0.4f, v[17] = 0.0f, v[18] = 0.0f, v[19] = 1.0f;	// stanga sus

		th[0] = -0.075f, th[1] = -0.7f, th[2] = 0.0f, th[3] = 0.0f, th[4] = 1.0f;	// stanga jos
		th[5] = 0.075f, th[6] = -0.7f, th[7] = 0.0f, th[8] = 1.0/16, th[9] = 1.0f;	// dreapta jos
		th[10] = 0.075f, th[11] = -0.4f, th[12] = 0.0f, th[13] = 1.0/16, th[14] = 0.0f;	// dreapta sus
		th[15] = -0.075f, th[16] = -0.4f, th[17] = 0.0f, th[18] = 0.0f, th[19] = 0.0f;	// stanga sus

		time = glfwGetTime();
		curr_tex = 0;
		changeVectors(texes[curr_tex]);
		level = lvl;
		if (level == 1) can_shoot = 1;
		if (level == 2) can_shoot = 0;
		if (level == 3) {
			v[0] = -32.0 / screen_width; v[1] = -32.0 / screen_height;
			v[5] = 32.0 / screen_width; v[6] = -32.0 / screen_height;
			v[10] = 32.0 / screen_width; v[11] = 32.0 / screen_height;
			v[15] = -32.0 / screen_width; v[16] = 32.0 / screen_height;
			can_shoot = 0;
		}
		if (level == 4) can_shoot = 0;
		Update();
	}

	void changeVectors(Loaded_Texture *t) {
		v[3] = (float)t->x / mw + (float)t->width / mw, v[4] = (float)t->y / mh + (float)t->height / mh;
		v[8] = (float)t->x / mw, v[9] = (float)t->y / mh + (float)t->height / mh;
		v[13] = (float)t->x / mw, v[14] = (float)t->y / mh;
		v[18] = (float)t->x / mw + (float)t->width / mw, v[19] = (float)t->y / mh;
	}

	void Update() {
		// ship
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
		// thrust
		thrust %= 16;
		th[0] = v[0] + 0.015; th[1] = v[1] - 0.26;
		th[5] = v[5] - 0.015; th[6] = v[6] - 0.26;
		th[10] = v[10] - 0.015; th[11] = v[11] - 0.26;
		th[15] = v[15] + 0.015; th[16] = v[16] - 0.26;

		if (thrust == 0) {
			th[8] = th[13] = 0;
			th[3] = th[18] = 1.0 / 16;
		}
		else {
			float step = 1.0 / 16;
			th[3] += step;
			th[8] += step;
			th[13] += step;
			th[18] += step;
		}
		++thrust;
		// bullets
		float ct = -0.02;
		if (rewind_state && glfwGetTime() - rewind > rewind_time) {
			v[1] -= ct, v[6] -= ct;
			v[11] -= ct, v[16] -= ct;
			rewind_state = 0;
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE) && can_shoot
			&& glfwGetTime() - last_shot > bullet_delay_time) {
			last_shot = glfwGetTime();
			bullets[nr_bullets++] = new Bullet(v, 1);
			v[1] += ct, v[6] += ct;
			v[11] += ct, v[16] += ct;
			rewind = glfwGetTime();
			rewind_state = 1;
	//		PlaySound
		}

		for (int i = 0; i < nr_bullets;) {
			bullets[i]->Update();
			if (bullets[i]->Offscreen()) {
				delete bullets[i];
				bullets[i] = bullets[--nr_bullets];
			}
			else i++;
		}
	}

	void Draw() {
		// ship
		glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), v, GL_DYNAMIC_DRAW);
		glUniform1i(tex_loc, 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		// thrust
		glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), th, GL_DYNAMIC_DRAW);
		glUniform1i(tex_loc, 10);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		// bullets
		for (int i = 0; i < nr_bullets; i++)
			bullets[i]->Draw();
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
		float ct = 0.023;
		if (v[0] > -1) {
			v[0] -= ct;
			v[5] -= ct;
			v[10] -= ct;
			v[15] -= ct;
		}
	}
	void moveright() {
		float ct = 0.023;
		if (v[10] < 1) {
			v[0] += ct;
			v[5] += ct;
			v[10] += ct;
			v[15] += ct;
		}
	}

	~Player() {
		for (int i = 0; i < 48; i++) {
			delete texes[i];
		}
		delete[] texes;
		for (int i = 0; i < nr_bullets; i++) {
			delete bullets[i];
		}
		delete[] bullets;
	}
};
// core
class AnimationManager {
public:
	Enemy** enemies; // enemy master vecto
	Bullet** ebullets; // enemy bullets master vector
	Resource** resources; // resource master vector
	Explosion** explosions; // explosion master vector

	int nr_enemies, nr_ebullets, nr_resources, nr_explosions;
	float spawn_time, spawn_delay, last_spawn;

	Player *player;

	unsigned int *index_buffer;
	GLuint elementbuffer;
	float* vertex_buffer;
	GLuint vbo;

	int flag; // 0, 1, 2, 3 (waiting screen, Lv 1, 2..)
	int bg_id, dir; // background id, intre 1,2,3 sau 4
	float *bg_buf, bg_timer;
	int astro_frame = 0;
	float astro_time;
	int click_flag;
	int score = 0;
	int current_song;

	AnimationManager() {
		enemies = new Enemy*[1000];
		ebullets = new Bullet*[1000];
		explosions = new Explosion*[1000];
		resources = new Resource*[1000];
		nr_enemies = nr_ebullets = nr_explosions = 0;
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
		click_flag = -1;
		PlaySound(TEXT("../Data/Wavs/homescreen.wav"), NULL, SND_ASYNC | SND_LOOP);
		current_song = 1;
	}

	void setPlayer(Player *p) {
		player = p;
	}

	void init_update(){
		click_flag = -1;
		bg_init();
		player->health = 1000;
		spawn_time = last_spawn = glfwGetTime();
		while (nr_ebullets) delete ebullets[--nr_ebullets];
		while (nr_enemies) delete enemies[--nr_enemies];
		while (nr_explosions) delete explosions[--nr_explosions];
		while (nr_resources) delete resources[--nr_resources];
	}

	void Update() {

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_0) || click_flag == 0
			|| GLFW_PRESS == glfwGetKey(window, GLFW_KEY_BACKSPACE)) {
			if (current_song != 1)
				PlaySound(TEXT("../Data/Wavs/homescreen.wav"), NULL, SND_ASYNC | SND_LOOP);
			current_song = 1;
			flag = 0;
			score = 0;
			bg_id = 1;
			init_update();
			player->init(0);
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_1) || click_flag == 1) {
			if (current_song != 2) {
				PlaySound(TEXT("../Data/Wavs/OK.wav"), NULL, SND_ASYNC);
				PlaySound(TEXT("../Data/Wavs/30.wav"), NULL, SND_ASYNC | SND_NOSTOP | SND_LOOP);
			}
			current_song = 2;
			flag = 1;
			score = 0;
			astro_time = glfwGetTime();
			bg_id = 2;
			init_update();
			spawn_delay = 0.5;
			player->init(1);
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_2) || click_flag == 2) {
			if (current_song != 3) {
				PlaySound(TEXT("../Data/Wavs/OK.wav"), NULL, SND_ASYNC);
				PlaySound(TEXT("../Data/Wavs/26.wav"), NULL, SND_ASYNC | SND_NOSTOP | SND_LOOP);
			}
			current_song = 3;
			flag = 2;
			score = 0;
			bg_id = 3;
			init_update();
			spawn_delay = 0.4;
			player->init(2);
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_3) || click_flag == 3) {
			flag = 3;
			bg_id = 4;
			init_update();
			player->init(0);
		}

		if (flag == 1) {
			PlaySound(TEXT("../Data/Wavs/30.wav"), NULL, SND_ASYNC | SND_NOSTOP | SND_LOOP);
			player->Update();
			for (int i = 0; i < nr_enemies;) {
				enemies[i]->Update(player->v);
				if (enemies[i]->Offscreen()) {
					delete enemies[i];
					enemies[i] = enemies[--nr_enemies];
				}
				else ++i;
			}
			if (glfwGetTime() - last_spawn > spawn_delay) {
				last_spawn = glfwGetTime();
				enemies[nr_enemies++] = new Enemy();
			}

			if (glfwGetTime() - spawn_time > 10) {
				spawn_delay *= 3.0 / 8;
				spawn_time = glfwGetTime();
			}
			ColPlayerBulletEnemyBullet();
			ColPlayerBulletEnemy();
			ColPlayerEnemyBullet();
			ColPlayerEnemy();
			// explosions update
			for (int i = 0; i < nr_explosions;) {
				explosions[i]->Update();
				if (explosions[i]->isTerminated()) {
					delete explosions[i];
					explosions[i] = explosions[--nr_explosions];
				}
				else ++i;
			}
			if (player->health <= 0) {
				flag = 3;
				bg_id = 4;
				init_update();
				player->init(0);
			}
			memcpy(vertex_buffer, player->v, 20 * sizeof(float));
		}

		if (flag == 2) {
			PlaySound(TEXT("../Data/Wavs/26.wav"), NULL, SND_ASYNC | SND_NOSTOP | SND_LOOP);
			player->Update();
			for (int i = 0; i < nr_resources;) {
				resources[i]->Update();
				if (resources[i]->Offscreen()) {
					delete resources[i];
					resources[i] = resources[--nr_resources];
				}
				else ++i;
			}
			if (glfwGetTime() - last_spawn > spawn_delay) {
				last_spawn = glfwGetTime();
				resources[nr_resources++] = new Resource();
			}

			if (glfwGetTime() - spawn_time > 10) {
				spawn_delay *= 7.0 / 8;
				spawn_time = glfwGetTime();
			}
			ColPlayerResource();
			if (player->health <= 0) {
				flag = 3;
				bg_id = 4;
				init_update();
				player->init(0);
			}
			memcpy(vertex_buffer, player->v, 20 * sizeof(float));
		}

		//bullets update
		for (int i = 0; i < nr_ebullets;) {
			ebullets[i]->Update();
			if (ebullets[i]->Offscreen()) {
				delete ebullets[i];
				ebullets[i] = ebullets[--nr_ebullets];
			}
			else ++i;
		}
		for (int i = 0; i < nr_enemies; i++) {
			if (enemies[i]->must_shoot) {
				ebullets[nr_ebullets++] = new Bullet(enemies[i]->v, 2);
				enemies[i]->must_shoot = 0;
			}
		}

		// background
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

	void scoreDraw(int f1) {
		if (score < 0) score = 0;
		if (f1 == 1 || f1 == 2) {
			float vs[20];
			vs[0] = -0.95f, vs[1] = 0.7f, vs[2] = 0.0f, vs[3] = 0.0f, vs[4] = 1.0f;	// stanga jos
			vs[5] = -0.7f, vs[6] = 0.7f, vs[7] = 0.0f, vs[8] = 1.0, vs[9] = 1.0f;	// dreapta jos
			vs[10] = -0.7f, vs[11] = 0.95f, vs[12] = 0.0f, vs[13] = 1.0, vs[14] = 0.0f;	// dreapta sus
			vs[15] = -0.95f, vs[16] = 0.95f, vs[17] = 0.0f, vs[18] = 0.0f, vs[19] = 0.0f;	// stanga sus
			if (f1 == 1) {
				glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vs, GL_DYNAMIC_DRAW);
				glUniform1i(tex_loc, 8);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
			}
			vs[0] = vs[15] = -0.7;
			vs[5] = vs[10] = -0.6;
			if (f1 == 2) {
				vs[0] = 0; vs[1] = -0.3;
				vs[5] = 0.1; vs[6] = -0.3;
				vs[10] = 0.1; vs[11] = -0.2;
				vs[15] = 0; vs[16] = -0.2;
			}
			int aux2 = score;
			int aux = 0;
			int zero = 0;
			if (score == 0) zero = 1;
			while (aux2 > 9 && aux2 % 10 == 0) {
				aux2 /= 10;
				++zero;
			}
			while (aux2) {
				int cif = aux2 % 10;
				aux *= 10;
				aux += cif;
				aux2 /= 10;
			}
			while (aux) {
				int cif = aux % 10;
				aux /= 10;
				vs[3] = vs[18] = (cif * 1.0 / 10) - 0.1;
				vs[8] = vs[13] = vs[3] + 0.1;
				if (cif == 0) {
					vs[3] = vs[18] = 1;
					vs[8] = vs[13] = 0.9;
				}
				glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vs, GL_DYNAMIC_DRAW);
				glUniform1i(tex_loc, 9);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
				vs[0] += 0.1; vs[10] += 0.1;
				vs[5] += 0.1; vs[15] += 0.1;
			}
			while (zero--) {
				vs[3] = vs[18] = 1;
				vs[8] = vs[13] = 0.9;
				glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vs, GL_DYNAMIC_DRAW);
				glUniform1i(tex_loc, 9);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
				vs[0] += 0.1; vs[10] += 0.1;
				vs[5] += 0.1; vs[15] += 0.1;
			}
		}
	}

	void drawHealth() {
		float vs[20];
		vs[0] = 0.3f, vs[1] = 0.7f, vs[2] = 0.0f, vs[3] = 0.0f, vs[4] = 1.0f;	// stanga jos
		vs[5] = 0.5f, vs[6] = 0.7f, vs[7] = 0.0f, vs[8] = 1.0, vs[9] = 1.0f;	// dreapta jos
		vs[10] = 0.5f, vs[11] = 0.95f, vs[12] = 0.0f, vs[13] = 1.0, vs[14] = 0.0f;	// dreapta sus
		vs[15] = 0.3f, vs[16] = 0.95f, vs[17] = 0.0f, vs[18] = 0.0f, vs[19] = 0.0f;	// stanga sus
		glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vs, GL_DYNAMIC_DRAW);
		glUniform1i(tex_loc, 21);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		vs[0] = vs[15] = 0.5;
		vs[5] = vs[10] = 0.6;
		int aux2 = player->health;
		int aux = 0;
		int zero = 0;
		if (aux2 == 0) zero = 1;
		while (aux2 > 9 && aux2 % 10 == 0) {
			aux2 /= 10;
			++zero;
		}
		while (aux2) {
			int cif = aux2 % 10;
			aux *= 10;
			aux += cif;
			aux2 /= 10;
		}
		while (aux) {

			int cif = aux % 10;
			aux /= 10;
			vs[3] = vs[18] = (cif * 1.0 / 10) - 0.1;
			vs[8] = vs[13] = vs[3] + 0.1;
			if (cif == 0) {
				vs[3] = vs[18] = 1;
				vs[8] = vs[13] = 0.9;
			}
			glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vs, GL_DYNAMIC_DRAW);
			glUniform1i(tex_loc, 9);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
			vs[0] += 0.1; vs[10] += 0.1;
			vs[5] += 0.1; vs[15] += 0.1;
		}
		while (zero--) {
			vs[3] = vs[18] = 1;
			vs[8] = vs[13] = 0.9;
			glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vs, GL_DYNAMIC_DRAW);
			glUniform1i(tex_loc, 9);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
			vs[0] += 0.1; vs[10] += 0.1;
			vs[5] += 0.1; vs[15] += 0.1;
		}
	}

	void Draw() {
		// background
		int bg_nr = 1;
		if (bg_id == 2) bg_nr = 2;
		glBufferData(GL_ARRAY_BUFFER, bg_nr * 20 * sizeof(float), bg_buf, GL_DYNAMIC_DRAW);
		glUniform1i(tex_loc, bg_id);
		glDrawElements(GL_TRIANGLES, bg_nr * 6, GL_UNSIGNED_INT, (void*)0);

		if (flag == 1) {
			scoreDraw(1);
		}
		if (flag == 2) {
			scoreDraw(1);
		}
		if (flag == 3) {
			scoreDraw(2);
		}

		// buttons, home screen
		if (flag == 0) {
			DrawHomeButtons();
		}
		// player
		if (flag != 0 && flag != 3) {
			player->Draw();
			drawHealth();
		}
		// enemies
		for (int i = 0; i < nr_enemies; i++)
			enemies[i]->Draw();
		// explosions
		for (int i = 0; i < nr_explosions; i++)
			explosions[i]->Draw();
		// bullets
		for (int i = 0; i < nr_ebullets; i++)
			ebullets[i]->Draw();
		// resources
		for (int i = 0; i < nr_resources; i++)
			resources[i]->Draw();
	}

	void bg_init() {
		bg_timer = glfwGetTime();
		if (bg_id == 1) {
			bg_buf[0] = -1, bg_buf[1] = -1, bg_buf[2] = 0.0f, bg_buf[3] = 0.3f, bg_buf[4] = 0.6f;	// stanga jos
			bg_buf[5] = 1, bg_buf[6] = -1, bg_buf[7] = 0.0f, bg_buf[8] = 0.6f, bg_buf[9] = 0.6f;	// dreapta jos
			bg_buf[10] = 1, bg_buf[11] = 1, bg_buf[12] = 0.0f, bg_buf[13] = 0.6f, bg_buf[14] = 0.3f;	// dreapta sus
			bg_buf[15] = -1, bg_buf[16] = 1, bg_buf[17] = 0.0f, bg_buf[18] = 0.3f, bg_buf[19] = 0.3f;	// stanga sus
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
		if (bg_id == 3 || bg_id == 4){
			bg_buf[0] = -1, bg_buf[1] = -1, bg_buf[2] = 0.0f, bg_buf[3] = 0.0f, bg_buf[4] = 1.0f;	// stanga jos
			bg_buf[5] = 1, bg_buf[6] = -1, bg_buf[7] = 0.0f, bg_buf[8] = 1.0f, bg_buf[9] = 1.0f;	// dreapta jos
			bg_buf[10] = 1, bg_buf[11] = 1, bg_buf[12] = 0.0f, bg_buf[13] = 1.0f, bg_buf[14] = 0.0f;	// dreapta sus
			bg_buf[15] = -1, bg_buf[16] = 1, bg_buf[17] = 0.0f, bg_buf[18] = 0.0f, bg_buf[19] = 0.0f;	// stanga sus
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
				if (bg_buf[4] < 1) { // up
					bg_buf[4] += ct; bg_buf[9] += ct; bg_buf[14] += ct; bg_buf[19] += ct;
				}
				else dir = 3;
			}
			if (dir == 3) {
				if (bg_buf[14] > 0) { // down
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

	void DrawHomeButtons() {
		float bt_buf[20];
		bt_buf[0] = -0.4f, bt_buf[1] = 0.3f, bt_buf[2] = 0.0f, bt_buf[3] = 0.0f, bt_buf[4] = 1.0f;	// stanga jos
		bt_buf[5] = 0.4f, bt_buf[6] = 0.3f, bt_buf[7] = 0.0f, bt_buf[8] = 1.0f, bt_buf[9] = 1.0f;	// dreapta jos
		bt_buf[10] = 0.4f, bt_buf[11] = 0.5f, bt_buf[12] = 0.0f, bt_buf[13] = 1.0f, bt_buf[14] = 0.0f;	// dreapta sus
		bt_buf[15] = -0.4f, bt_buf[16] = 0.5f, bt_buf[17] = 0.0f, bt_buf[18] = 0.0f, bt_buf[19] = 0.0f;	// stanga sus

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = 768 - y;
		x /= 512; x -= 1; y /= 384; y -= 1;

		if (x >= bt_buf[0] && x <= bt_buf[5] && y >= bt_buf[1] && y <= bt_buf[11]) {
			if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) click_flag = 1;
			glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
		}
		else glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bt_buf), bt_buf, GL_DYNAMIC_DRAW);
		glUniform1i(tex_loc, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

		bt_buf[1] -= 0.3; bt_buf[6] -= 0.3; bt_buf[11] -= 0.3; bt_buf[16] -= 0.3;
		if (x >= bt_buf[0] && x <= bt_buf[5] && y >= bt_buf[1] && y <= bt_buf[11]) {
			if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) click_flag = 2;
			glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
		}
		else glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bt_buf), bt_buf, GL_DYNAMIC_DRAW);
		glUniform1i(tex_loc, 7);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // back to 
		if (flag == 3) scoreDraw(2);

		// Astro
		astro_frame %= 6;
		bt_buf[0] = -0.9f, bt_buf[1] = -0.6f, bt_buf[2] = 0.0f, bt_buf[3] = (float)astro_frame / 6, bt_buf[4] = 1.0f;	// stanga jos
		bt_buf[5] = -0.4f, bt_buf[6] = -0.6f, bt_buf[7] = 0.0f, bt_buf[8] = (float)astro_frame / 6 + 1.0 / 6, bt_buf[9] = 1.0f;	// dreapta jos
		bt_buf[10] = -0.4f, bt_buf[11] = 0.6f, bt_buf[12] = 0.0f, bt_buf[13] = (float)astro_frame / 6 + 1.0 / 6, bt_buf[14] = 0.0f;	// dreapta sus
		bt_buf[15] = -0.9f, bt_buf[16] = 0.6f, bt_buf[17] = 0.0f, bt_buf[18] = (float)astro_frame / 6, bt_buf[19] = 0.0f;	// stanga sus
		glBufferData(GL_ARRAY_BUFFER, sizeof(bt_buf), bt_buf, GL_DYNAMIC_DRAW);
		if (glfwGetTime() - astro_time > 0.2) {
			astro_time = glfwGetTime();
			++astro_frame;
		}
		glUniform1i(tex_loc, 17);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	}

	// collisions
	void ColPlayerBulletEnemyBullet() {
		for (int i = 0; i < nr_enemies; i++) { // fiecare inamic
			for (int j = 0; j < nr_ebullets;) { // glont al inamicului
				int intersect = 1;
				for (int k = 0; k < player->nr_bullets;) { // glont al player-ului
					if (square_intersect(ebullets[j]->v, player->bullets[k]->v)) {
						explosions[nr_explosions++] = new Explosion(3, 
							(ebullets[j]->v[0] + player->bullets[k]->v[5]) / 2,
							(ebullets[j]->v[1] + player->bullets[k]->v[16]) / 2);

						delete ebullets[j];
						delete player->bullets[k];
						ebullets[j] = ebullets[--nr_ebullets];
						player->bullets[k] = player->bullets[--player->nr_bullets];
						intersect = 0;
					}
					else k++;
				}
				j += intersect;
			}
		}
	}

	void ColPlayerBulletEnemy() {
		for (int i = 0; i < nr_enemies;) { // fiecare inamic
			int intersect = 1;
			for (int j = 0; j < player->nr_bullets;) { // glont al player-ului
				if (square_intersect(enemies[i]->v, player->bullets[j]->v)) {
					enemies[i]->health -= 40;
					score += 40;
					if (enemies[i]->health <= 0) {
						explosions[nr_explosions++] = new Explosion(2,
							(enemies[i]->v[0] + enemies[i]->v[5]) / 2,
							(enemies[i]->v[1] + enemies[i]->v[16]) / 2);
					}

					delete player->bullets[j];
					player->bullets[j] = player->bullets[--player->nr_bullets];

					if (enemies[i]->health <= 0) {
						delete enemies[i];
						enemies[i] = enemies[--nr_enemies];
					}
					else j++;
				}
				else j++;
			}
			i += intersect;
		}
	}

	void ColPlayerEnemyBullet() {
		for (int i = 0; i < nr_enemies; i++) {
			for (int j = 0; j < nr_ebullets;) {
				if (square_intersect(player->v, ebullets[j]->v)) {
					score -= 20;
					explosions[nr_explosions++] = new Explosion(1,
						(ebullets[j]->v[0] + player->v[5]) / 2,
						(ebullets[j]->v[1] + player->v[16]) / 2);
					player->health -= 20;
					delete  ebullets[j];
					ebullets[j] = ebullets[--nr_ebullets];
				}
				else ++j;
			}
		}
	}

	void ColPlayerEnemy() {
		for (int i = 0; i < nr_enemies;) {
			if (square_intersect(player->v, enemies[i]->v)) {
				player->health -= 100;
				score -= 50;
				explosions[nr_explosions++] = new Explosion(2,
					(enemies[i]->v[0] + player->v[5]) / 2,
					(enemies[i]->v[1] + player->v[16]) / 2);
				delete  enemies[i];
				enemies[i] = enemies[--nr_enemies];
			}
			else i++;
		}
	}

	void ColPlayerResource() {
		for (int i = 0; i < nr_resources;) {
			if (square_intersect(player->v, resources[i]->v)) {
				if (resources[i]->type == 1) {
					score -= 300 * resources[i]->size;
					player->health -= 1500 * resources[i]->size;
				}
				if (resources[i]->type == 2) score += 750 * resources[i]->size;
				delete  resources[i];
				resources[i] = resources[--nr_resources];
			}
			else i++;
		}
	}

	int square_intersect(float *a, float *b) {
		float ct = 0.02;
	//	if (a.max.x < b.min.x or a.min.x > b.max.x) return false
	//	if (a.max.y < b.min.y or a.min.y > b.max.y) return false
		if (a[5] - ct < b[0] + ct || a[0] + ct > b[5] - ct) return 0;
		if (a[11] - ct < b[1] + ct || a[1] + ct > b[11] - ct) return 0;
		return 1;
	}

	~AnimationManager() {
		for (int i = 0; i < nr_enemies; i++)
			delete enemies[i];
		delete[] enemies;

		for (int i = 0; i < nr_ebullets; i++)
			delete ebullets[i];
		delete[] ebullets;

		for (int i = 0; i < nr_explosions; i++)
			delete explosions[i];
		delete[] explosions;

		for (int i = 0; i < nr_resources; i++)
			delete resources[i];
		delete[] resources;

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

	s->setPlayer(new Player());
	//	PlaySound(TEXT("03.wav"), NULL, SND_ASYNC);

	float prevTime = glfwGetTime();
	const float frame_time = 1.0 / 60;
	float accumulator = 0;
	glDisable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		//..... Randare................. 
		float time1 = glfwGetTime();
		// FPS counter
		_update_fps_counter(window);

		// time control
		float deltaTime = (float)(glfwGetTime() - prevTime);
		prevTime = glfwGetTime();
		accumulator += deltaTime;
		//----------

		while (accumulator >= frame_time) {
			s->Update();
			accumulator -= frame_time;
		}
		s->Draw();

		// facem swap la buffere (Double buffer)
		glfwSwapBuffers(window);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		while (glfwGetTime() - time1 < 1.0 / 60);
	}

	glDetachShader(shader_programme, vs);
	glDetachShader(shader_programme, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

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
	LoadImg("../Data/Sprites/bt1.png", 6);
	LoadImg("../Data/Sprites/bt2.png", 7);
	LoadImg("../Data/Sprites/score.png", 8);
	LoadImg("../Data/Sprites/numbers.png", 9);
	LoadImg("../Data/Sprites/thrust.png", 10);
	LoadImg("../Data/Sprites/projectile2.png", 11);
	LoadImg("../Data/Sprites/projectile1.png", 12); 
	LoadImg("../Data/Sprites/strip_scythe.png", 13);
	LoadImg("../Data/Sprites/Exp_type_A.png", 14);
	LoadImg("../Data/Sprites/Exp_type_B.png", 15);
	LoadImg("../Data/Sprites/Exp_type_C.png", 16);
	LoadImg("../Data/Sprites/astro.png", 17);
	LoadImg("../Data/Sprites/strip_fighter.png", 18);
	LoadImg("../Data/Sprites/gun_blast.png", 19);
	LoadImg("../Data/Sprites/gun_blast_blue.png", 20);
	LoadImg("../Data/Sprites/health.png", 21);
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
		sprintf(tmp, "O @ fps: %.2f", fps);
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

/*
			__
		   / _)    rawr!
	.-^^^-/ /
 __/       /
<__.|_|-|_|

*/
