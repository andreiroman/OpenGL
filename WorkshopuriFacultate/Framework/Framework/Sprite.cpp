#include "common.h"
#include "Sprite.h"





GLubyte indices[] = {	0, 1, 2, // first triangle (bottom left - top left - top right)
						0, 2, 3 }; // second triangle (bottom left - top right - bottom right)

CSprite::CSprite()
{
	shader_programme = 0;
	vao = 0;
	position_vbo = 0;
	textureCoordinates_vbo = 0;
	indexBuffer = 0;
	angle = 0.f;
	scale = 1.0f;
}

CSprite::CSprite(const CSprite &r)
{
	transf_loc = r.transf_loc;
	transf = r.transf;
	isAlive = r.isAlive;
	position = r.position;
	angle = r.angle;
	scale = r.scale;

	shader_programme = r.shader_programme;
	vao = r.vao;
	position_vbo = r.position_vbo;
	textureCoordinates_vbo = r.textureCoordinates_vbo;
	indexBuffer = r.indexBuffer;
	aspect_ratio = r.aspect_ratio;
	texture = r.texture;

}

void CSprite::Init(unsigned int shader_prg, float *uv,float ar,float rscale)
{
	aspect_ratio = ar;
	GLfloat Vertices[] = { 0.0f, 0.0f, 0.0f,
		ar, 0.0f, 0.0f,
	    ar, 1.0f , 0.0f,
		0.0f, 1.0f , 0.0f };

	scale = rscale;
	transf.eye();

	glGenBuffers(1, &position_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof (float), Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &textureCoordinates_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordinates_vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof (float), uv, GL_STATIC_DRAW);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned char), &indices, GL_STATIC_DRAW);

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(TEXTURE_COORDINATE_0);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordinates_vbo);
	glVertexAttribPointer(TEXTURE_COORDINATE_0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	
	shader_programme = shader_prg;
	

}

void CSprite::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUseProgram(shader_programme);
	glBindVertexArray(vao);
	
	glEnableVertexAttribArray(POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(TEXTURE_COORDINATE_0);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordinates_vbo);
	glVertexAttribPointer(TEXTURE_COORDINATE_0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	transf.eye();
	cMatrix4f pivot;
	cMatrix4f rotate;
	cMatrix4f translate;
	cMatrix4f scalem;
	
	pivot.eye();
	rotate.eye();
	translate.eye();
	scalem.eye();

	scalem.Scale(scale, scale, scale);

	rotate.Rotate(angle, 0.0f, 0.0f, 1.0f);

	translate.Translate(position.x, position.y, position.z);
	transf = ((pivot * scalem) *rotate) * translate;
	
	transf_loc = glGetUniformLocation(shader_programme, "u_transfMatrix");
	glUniformMatrix4fv(transf_loc, 1, GL_FALSE, (GLfloat*)&transf.m[0][0]);

	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawElements(
		GL_TRIANGLES,      // mode
		6,    // count
		GL_UNSIGNED_BYTE,   // type
		(void*)0           // element array buffer offset
		);
}

CSprite::~CSprite()
{

}