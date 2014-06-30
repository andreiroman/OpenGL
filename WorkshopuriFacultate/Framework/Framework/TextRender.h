#ifndef __H_TEXT_RENDERER
#define __H_TEXT_RENDERER

class TextRender
{
	GLuint pointsVBO;
	GLuint textureVBO;
	int pointCount;

	struct glyph_data
	{
		int ascii_code;
		float prop_xMin;
		float prop_width;
		float prop_yMin;
		float prop_height;
		float prop_y_offset;
		// draw data
		float g_width;
		float y_offset;

		glyph_data()
		{
			ascii_code = -1;
			prop_xMin = 0.0f;
			prop_width = 0.0f;
			prop_yMin = 0.0f;
			prop_height = 0.0f;
			prop_y_offset = 0.0f;
		}
	};

	struct font_data
	{
		int atlas_cols;
		int atlas_rows;
		int texture_id;
		int shader_id;
		glyph_data m_fontGlyphs[256];

		font_data()
		{
			atlas_cols = 16;
			atlas_rows = 16;
		}
	};

	font_data CurrentFont;


	void StringToVBO(
		const char* str,
		float at_x,
		float at_y,
		float scale_px,
		GLuint* points_vbo,
		GLuint* texcoords_vbo,
		int* point_count
		);

public:
	// loads the font data
	void LoadFont(const char *filename);
	bool LoadFontMetadata(const char* meta_file);
	bool LoadFontTexture(const char* texture_file);

	void DrawString(const char*str, float at_x, float at_y, float scale_px);
	void Init();
};

#endif