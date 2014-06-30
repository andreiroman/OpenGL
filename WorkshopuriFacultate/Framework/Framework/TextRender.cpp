#include "common.h"
#include "Utils.h"
#include "TextRender.h"

#define LINE_HEIGHT 1.0f

void TextRender::Init()
{
	pointsVBO = 0;
	textureVBO = 0;
	pointCount = 0;

	glGenBuffers(1, &pointsVBO);
	glGenBuffers(1, &textureVBO);

}

void TextRender::DrawString(const char*str, float at_x, float at_y, float scale_px)
{
	StringToVBO(str, at_x, at_y, scale_px, &pointsVBO, &textureVBO, &pointCount);
}

void TextRender::StringToVBO(
	const char* str,
	float at_x,
	float at_y,
	float scale_px,
	GLuint* points_vbo,
	GLuint* texcoords_vbo,
	int* point_count
	)
{
	float initial_at_x = at_x;
	int tex_loc = glGetUniformLocation(CurrentFont.shader_id, "tex");
	glUseProgram(CurrentFont.shader_id);
	glUniform1i(tex_loc, 0); // use active texture 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CurrentFont.texture_id);


	int len = strlen(str);
	
	float* points_tmp = new float[len * 12];
	float* texcoords_tmp = new float[len * 12];

	for (int i = 0; i < len; i++)
	{
		// get ascii code as integer
		int ascii_code = str[i];
		if (ascii_code == '\n')
		{
			at_x = initial_at_x;
			at_y -= scale_px / g_gl_height *
				LINE_HEIGHT;
			continue;
		}
		// work out row and column in atlas
		int atlas_col = (ascii_code - ' ') % CurrentFont.atlas_cols;
		int atlas_row = (ascii_code - ' ') / CurrentFont.atlas_cols;

		// work out texture coordinates in atlas
		float s = atlas_col * (1.0f / CurrentFont.atlas_cols);
		float t = (atlas_row + 1) * (1.0f / CurrentFont.atlas_rows);

		// work out position
 		float x_pos = at_x;
		float y_pos = at_y - scale_px / g_gl_height *
			CurrentFont.m_fontGlyphs[ascii_code].y_offset;

		// move next glyph along to the end of this one
		if (i + 1 < len) {
			// upper-case letters move twice as far
			at_x += CurrentFont.m_fontGlyphs[ascii_code].g_width * scale_px / g_gl_width;
		}

		// add 6 points and texture coordinates to buffers for each glyph
		points_tmp[i * 12] = x_pos;
		points_tmp[i * 12 + 1] = y_pos;
		points_tmp[i * 12 + 2] = x_pos;
		points_tmp[i * 12 + 3] = y_pos - scale_px / g_gl_height;
		points_tmp[i * 12 + 4] = x_pos + scale_px / g_gl_width;
		points_tmp[i * 12 + 5] = y_pos - scale_px / g_gl_height;

		points_tmp[i * 12 + 6] = x_pos + scale_px / g_gl_width;
		points_tmp[i * 12 + 7] = y_pos - scale_px / g_gl_height;
		points_tmp[i * 12 + 8] = x_pos + scale_px / g_gl_width;
		points_tmp[i * 12 + 9] = y_pos;
		points_tmp[i * 12 + 10] = x_pos;
		points_tmp[i * 12 + 11] = y_pos;

		
		texcoords_tmp[i * 12] = s;
		texcoords_tmp[i * 12 + 1] = 1.0f - t + 1.0f / CurrentFont.atlas_rows;
		texcoords_tmp[i * 12 + 2] = s;
		texcoords_tmp[i * 12 + 3] = 1.0f - t;
		texcoords_tmp[i * 12 + 4] = s + 1.0f / CurrentFont.atlas_cols;
		texcoords_tmp[i * 12 + 5] = 1.0f - t;

		texcoords_tmp[i * 12 + 6] = s + 1.0f / CurrentFont.atlas_cols;
		texcoords_tmp[i * 12 + 7] = 1.0f - t;
		texcoords_tmp[i * 12 + 8] = s + 1.0f / CurrentFont.atlas_cols;
		texcoords_tmp[i * 12 + 9] = 1.0f - t + 1.0f / CurrentFont.atlas_rows;
		texcoords_tmp[i * 12 + 10] = s;
		texcoords_tmp[i * 12 + 11] = 1.0f - t + 1.0f / CurrentFont.atlas_rows;




	} // endfor

	

	
	glBindBuffer(GL_ARRAY_BUFFER, *points_vbo);
	glEnableVertexAttribArray(POSITION);
	glBufferData(
		GL_ARRAY_BUFFER,
		len * 12 * sizeof (float),
		points_tmp,
		GL_DYNAMIC_DRAW
		);
	
	glVertexAttribPointer(POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, *texcoords_vbo);
	glEnableVertexAttribArray(TEXTURE_COORDINATE_0);
	glBufferData(
		GL_ARRAY_BUFFER,
		len * 12 * sizeof (float),
		texcoords_tmp,
		GL_DYNAMIC_DRAW
		);
	glVertexAttribPointer(TEXTURE_COORDINATE_0, 2, GL_FLOAT, GL_FALSE, 0, NULL);



	delete [] points_tmp;
	delete [] texcoords_tmp;
	*point_count = len * 6;


	glDrawArrays(GL_TRIANGLES, 0, *point_count);
}


bool TextRender::LoadFontMetadata(const char* meta_file)
{
	FILE* fp = fopen(meta_file, "r");
	if (!fp) {
		fprintf(stderr, "ERROR: could not open file %s\n", meta_file);
		return false;
	}
	char line[128];
	int ascii_code = -1;
	float prop_xMin = 0.0f;
	float prop_width = 0.0f;
	float prop_yMin = 0.0f;
	float prop_height = 0.0f;
	float prop_y_offset = 0.0f;
	// get header line first
	fgets(line, 128, fp);
	// loop through and get each glyph's info
	while (EOF != fscanf(
		fp, "%i %f %f %f %f %f\n",
		&ascii_code,
		&prop_xMin,
		&prop_width,
		&prop_yMin,
		&prop_height,
		&prop_y_offset
		)) {
		CurrentFont.m_fontGlyphs[ascii_code].ascii_code = ascii_code;
		CurrentFont.m_fontGlyphs[ascii_code].prop_xMin = prop_xMin;
		CurrentFont.m_fontGlyphs[ascii_code].prop_width = prop_width;
		CurrentFont.m_fontGlyphs[ascii_code].prop_yMin = prop_yMin;
		CurrentFont.m_fontGlyphs[ascii_code].prop_height = prop_height;
		CurrentFont.m_fontGlyphs[ascii_code].prop_y_offset = prop_y_offset;
		CurrentFont.m_fontGlyphs[ascii_code].g_width = prop_width;
		CurrentFont.m_fontGlyphs[ascii_code].y_offset = 1.0f - prop_height - prop_y_offset;
	}
	fclose(fp);
	return true;
}

bool TextRender::LoadFontTexture(const char* texture_file)
{
	int width,height;
	CurrentFont.texture_id = LoadTexture(texture_file,width,height);
	return true;
}

void TextRender::LoadFont(const char *filename)
{
	char formatstr[512];
	sprintf(formatstr, "%s.meta", filename);
	LoadFontMetadata(formatstr);
	sprintf(formatstr, "%s.png", filename);
	LoadFontTexture(formatstr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Load shaders from files
	const char* fragment_shader = LoadFileInMemory("../data/textFragmentShader.glsl");
	const char* vertex_shader = LoadFileInMemory("../data/textVertexShader.glsl");

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	CurrentFont.shader_id = glCreateProgram();
	glAttachShader(CurrentFont.shader_id, fs);
	glAttachShader(CurrentFont.shader_id, vs);
	glLinkProgram(CurrentFont.shader_id);

	delete[] fragment_shader;
	delete[] vertex_shader;
}
