#include "common.h"
#include "Utils.h"
#include "SpriteManager.h"
#include "Sprite.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include <algorithm>

CSpriteManager CSpriteManager::g_me;

void CSpriteManager::Loaded_Texture::set_texture_name(const char *name)
{
	int namelen = strlen(name);
	assert(namelen < MAX_TEXTURE_NAME);
	strcpy(texture_name, name);
}

void CSpriteManager::load_all_sprites()
{
	char dataPath[] = "../data/SpriteSheets/";
	std::vector<std::string> all_sprites_names;
	{
		std::string location(dataPath);
		location.append("all_sprite_sheets.xml");
		rapidxml::file<> xmlFile(location.c_str()); // Default template is char
		rapidxml::xml_document<> doc;
		doc.parse<0>(xmlFile.data());
		rapidxml::xml_node<> *pRoot = doc.first_node();
		
		for (rapidxml::xml_node<> *pNode = pRoot->first_node("SpriteSheet"); pNode; pNode = pNode->next_sibling())
		{
			all_sprites_names.push_back(pNode->first_attribute("file")->value());
		}

	}
	const std::vector<std::string>::iterator begin	= all_sprites_names.begin();
	const std::vector<std::string>::iterator end		= all_sprites_names.end();

	for (std::vector<std::string>::iterator it = begin; it != end; it++)
	{
		std::string locationSpr(dataPath);
		locationSpr.append(*it);
		rapidxml::file<> xmlFile(locationSpr.c_str()); // Default template is char
		rapidxml::xml_document<> doc;
		doc.parse<0>(xmlFile.data());
		rapidxml::xml_node<> *pRoot = doc.first_node();

		rapidxml::xml_attribute<> *img = pRoot->first_attribute("imagePath");
		assert(img);
		char *imageName = img->value();

		// find image in Loaded textures
		std::map<std::string, Loaded_Texture*>::iterator fnd = loadedTextures.find(imageName);
		Loaded_Texture* needed_img = NULL;
		if (fnd == loadedTextures.end()) // did not find image
		{
			std::string locationTex(dataPath);
			locationTex.append(imageName);
			int width, height;
			int texture = LoadTexture(locationTex.c_str(),width,height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			needed_img = new Loaded_Texture();
			needed_img->set_texture_id(texture);
			needed_img->set_texture_name(imageName);
			needed_img->set_height(height);
			needed_img->set_width(width);
			
			std::pair<std::string, Loaded_Texture*> ins_t(imageName, needed_img);
			loadedTextures.insert(ins_t);
		}
		else
		{
			needed_img = fnd->second;
		}
		
		for (rapidxml::xml_node<> *pNode = pRoot->first_node("SubTexture"); pNode; pNode = pNode->next_sibling())
		{

			Dict_Entry * new_entry = new Dict_Entry();
			rapidxml::xml_attribute<> *spr_name = pNode->first_attribute("name");
			strcpy(new_entry->sprite_name, spr_name->value());

			rapidxml::xml_attribute<> *spr_x = pNode->first_attribute("x");
			assert(spr_x);
			new_entry->x = atof(spr_x->value());

			rapidxml::xml_attribute<> *spr_y = pNode->first_attribute("y");
			assert(spr_y);
			new_entry->y = atof(spr_y->value());
			
			rapidxml::xml_attribute<> *spr_w = pNode->first_attribute("width");
			assert(spr_w);
			new_entry->width = atof(spr_w->value());

			rapidxml::xml_attribute<> *spr_h = pNode->first_attribute("height");
			assert(spr_h);
			new_entry->height = atof(spr_h->value());
			
			new_entry->refTex = needed_img->get();

			std::pair<std::string, Dict_Entry * > ins(new_entry->sprite_name, new_entry);
			loadedSprites.insert(ins);
		}
		doc.clear();
	}

}

CSpriteManager::CSpriteManager()
{
}

void CSpriteManager::Init()
{
	load_all_sprites();

	//Load shaders from files
	const char* fragment_shader = LoadFileInMemory("../data/fragmentShader.glsl");
	const char* vertex_shader = LoadFileInMemory("../data/vertexShader.glsl");

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	global_shader_program = glCreateProgram();
	glAttachShader(global_shader_program, fs);
	glAttachShader(global_shader_program, vs);
	glLinkProgram(global_shader_program);

	delete[] fragment_shader;
	delete[] vertex_shader;

	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	int tex_loc = glGetUniformLocation(global_shader_program, "basic_texture");
	glUseProgram(global_shader_program);
	glUniform1i(tex_loc, 0); // use active texture 0


}

CSprite * CSpriteManager::AddSprite(const char * name)
{
	CSprite *newSprite = new CSprite();
	std::map<std::string, Dict_Entry*>::iterator fnd =  loadedSprites.find(name);
	assert(fnd != loadedSprites.end());
	
	allSprites.push_back(newSprite);
	int idx = 0;

	GLfloat TexCoord[] = { 0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};
	Dict_Entry * entry = fnd->second;
	float w = entry->refTex->get_width();
	float h = entry->refTex->get_height();
	float uv[8];
	
	uv[0] = entry->x / w;					uv[1] = 1.0f - (entry->y + entry->height) / h;
	uv[2] = (entry->x + entry->width) / w;	uv[3] = 1.0f - (entry->y + entry->height) / h;
	uv[4] = (entry->x + entry->width) / w;	uv[5] = 1.0f - entry->y / h;
	uv[6] = entry->x / w;					uv[7] = 1.0f - entry->y / h;
	
	float ar = entry->width / entry->height;
	float sc = entry->height / g_gl_height;

	newSprite->Init(global_shader_program, uv, ar, sc);
	newSprite->SetTexture(fnd->second->refTex->get_texture_id());
	return newSprite;
}

void CSpriteManager::RemoveSprite(CSprite * spr)
{
	std::vector<CSprite *>::iterator it = allSprites.begin();
	std::vector<CSprite *>::iterator itend = allSprites.end();
	for (; it != itend; it++)
	{

		if ((*it) == spr)
		{
			allSprites.erase(it);
			break;
		}
	}
}

bool SpriteDepthCompare(CSprite* sp1, CSprite* sp2)
{
	return sp1->GetPosition().z < sp2->GetPosition().z;
}

void CSpriteManager::Draw()
{
	std::sort(allSprites.begin(), allSprites.end(), SpriteDepthCompare);

	std::vector<CSprite* >::iterator it = allSprites.begin();
	std::vector<CSprite* >::iterator itend = allSprites.end();
	for (; it != itend; it++)
	{
		projection_matrix.eye();
		//projection_matrix.Perspective(90, g_gl_width / g_gl_height, 1.0f, 1000.0f);

		projection_matrix.Ortho(SCREEN_LEFT, SCREEN_RIGHT, SCREEN_BOTTOM, SCREEN_TOP, 0.1f, 1000.0f);

		int proj_loc = glGetUniformLocation(global_shader_program, "u_proj_matrix");
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix.m[0][0]);

		glUseProgram(global_shader_program);
		(*it)->Draw();
	}
}

CSpriteManager::~CSpriteManager()
{
	std::map<std::string, Dict_Entry*>::iterator begin = loadedSprites.begin();
	std::map<std::string, Dict_Entry*>::iterator end = loadedSprites.end();
	std::map<std::string, Dict_Entry*>::iterator it;
	for (it = begin; it != end; it++)
	{
		delete(it->second);
	}
	std::map<std::string, Loaded_Texture*>::iterator begin_t = loadedTextures.begin();
	std::map<std::string, Loaded_Texture*>::iterator end_t = loadedTextures.end();
	std::map<std::string, Loaded_Texture*>::iterator it_t;
	for (it_t = begin_t; it_t != end_t; it_t++)
	{
		delete(it_t->second);
	}


	for (int i = 0; i < allSprites.size(); i++)
	{
		delete allSprites[i];
	}
	allSprites.clear();
}