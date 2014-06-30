#ifndef ___H_SPRITE_MANAGER
#define ___H_SPRITE_MANAGER

#include <vector>
#include <map>
#include "Matrix4.h"

class CSprite;

#define MAX_SPRITE_NAME 512
#define MAX_TEXTURE_NAME 512

class CSpriteManager
{
	class Loaded_Texture
	{
		int		num_ref;
		char	texture_name[MAX_TEXTURE_NAME];
		int		texture_id;
		int		height;
		int		width;
	public:

		Loaded_Texture()
		{
			num_ref = 0;
			texture_name[0] = '\0';
			texture_id = 0;
			height = 0;
			width  = 0;
		}

		Loaded_Texture *get(){ num_ref++; return this; }
		void drop(){ num_ref--; }

		void set_texture_name(const char *name);
		void set_texture_id(int id) { texture_id = id; }
		int  get_texture_id(){ return texture_id; }

		void set_width(int w){ width = w; }
		int get_width(){ return width; }

		void set_height(int h){ height = h; }
		int get_height(){ return height; }
	};

	struct Dict_Entry
	{
		char sprite_name[MAX_SPRITE_NAME];
		Loaded_Texture *refTex;
		float x, y, width, height;
		
		Dict_Entry()
		{
			refTex = 0;
			x = y = width = height = 0.0f;
			sprite_name[0] = '/0';
		}
	};

	std::map<std::string, Loaded_Texture*>	loadedTextures;
	std::map<std::string, Dict_Entry*>		loadedSprites;
	
	std::vector<CSprite *>					allSprites;

private:
	unsigned int global_shader_program;
	int			 global_texture;
	
	cMatrix4f	projection_matrix;
	void load_all_sprites();

public:
	CSpriteManager();
	~CSpriteManager();

	void Init();
	CSprite * AddSprite(const char* name);
	void RemoveSprite(CSprite* spr);
	void Draw();

	static CSpriteManager * Get()
	{
		return &g_me;
	}
	static CSpriteManager g_me;
};

#endif //___H_SPRITE_MANAGER
