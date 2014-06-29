#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <cstring>
#include <math.h>
#define PI 3.14159265

// functie banala de incarcat continutul unui fisier intr-un buffer
char * LoadFileInMemory(const char *filename)
{
	int size = 0;
	char *buffer = NULL;
	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = new char[size + 1];
	if (size != fread(buffer, sizeof(char), size, f)) {
		delete[] buffer;
	}
	fclose(f);
	buffer[size] = 0;
	return buffer;
}


int main() {
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

	int n;
	printf("Introduceti nr de puncte: ");
	scanf("%d", &n);
	// buffer cu vertecsi in RAM 
	float vertex_buffer[10000];
	memset(vertex_buffer, 0, 10000 * sizeof(float));
	float p_vect[3000];
	// n puncte pe circumferinta cercului deci n - 2 triunghiuri
	for (int i = 0; i <= n; i++) {
		p_vect[i] = ((float)i / n) * (2 * PI);
	}
	++n;
	p_vect[n] = p_vect[1];
	int count = 0;
	// introduc coordonatele cercurilor in buffer
	for (int i = 0; i <= n; i++) {
		vertex_buffer[count++] = sin(p_vect[i]) / 2; // coordonata OX
		vertex_buffer[count++] = cos(p_vect[i]) / 2; // coordonata OY
		count++; // coordonata OZ
		if (i % 2 == 1) {
			count += 3;
		}
	}
/*	printf("\nAm generat punctele:\n");
	for (int i = 0; i <= n; i++) {
		printf("%f, ", p_vect[i]);
	}
	printf("\nBuffer:\n");
	for (int i = 0; i < n + n/2 + 2; i++) {
		printf("%f %f %f\n", vertex_buffer[3 * i], vertex_buffer[3 * i + 1], vertex_buffer[3 * i + 2]);
	}
*/
	// Generam un buffer in memoria video si scriem in el punctele din ram
	GLuint vbo = 0;
	glGenBuffers(1, &vbo); // generam un buffer 
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // setam bufferul generat ca bufferul curent 
	glBufferData(GL_ARRAY_BUFFER, 5 * n * sizeof(float), vertex_buffer, GL_STATIC_DRAW); //  scriem in bufferul din memoria video informatia din bufferul din memoria RAM

	// De partea aceasta am uitat sa va spun la curs -> Pentru a defini bufferul alocat de opengl ca fiind buffer de in de atribute, stream de vertecsi trebuie sa :
	// 1. Ii spunem OpenGL-ului ca vom avea un slot pentru acest atribut (in cazul nostru 0) , daca mai aveam vreun atribut ar fi trebuit si acela enablat pe alt slot (de exemplu 1)
	// 2. Definit bufferul ca Vertex Attribute Pointer cu glVertexAttribPointer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// culori
	float g_color_buffer_data[10000];
	for (int i = 0; i < count; i += 3) {
		if (i < count/2 || count - i < 4) {
			g_color_buffer_data[i] = 1.0f;
			g_color_buffer_data[i + 1] = 0.0f;
		}
		else {
			g_color_buffer_data[i] = 0.0f;
			g_color_buffer_data[i + 1] = 1.0f;
		}
		g_color_buffer_data[i + 2] = 0.0f;
	}
	// 
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	while (!glfwWindowShouldClose(window)) {
		//..... Randare................. 
		// stergem ce s-a desenat anterior
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// spunem ce shader vom folosi pentru desenare
		glUseProgram(shader_programme);
		// facem bind la vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// draw points 0-3 from the currently bound VAO with current in-use shader
		for (int i = 0; i < n + n / 2; i%3==1?i+=2:i++) {
			glDrawArrays(GL_TRIANGLES, i, 3);

		}
//		glDrawArrays(GL_TRIANGLE_STRIP, 0, n + n / 2);
		// facem swap la buffere (Double buffer)
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}