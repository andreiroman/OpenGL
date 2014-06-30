#include "Matrix4.h"

class CSprite
{	
	unsigned int transf_loc;
	cMatrix4f transf;
	bool isAlive;
// Logic variables
	cVector3df position;
	float angle;
	float scale;
	float aspect_ratio;

// Graphic variables
	unsigned int shader_programme;
	unsigned int vao;
	unsigned int position_vbo;
	unsigned int textureCoordinates_vbo;
	unsigned int indexBuffer;

	int			texture;

	

public:
	CSprite();
	CSprite(const CSprite &r);

	void Init(unsigned int shader_prg, float *uv, float ar, float rscale);
	void Draw();

	float GetAspectRatio(){ return aspect_ratio; }

	bool GetIsAlive(){ return isAlive; }
	void SetIsAlive(bool is){ isAlive = is; }

	void SetTexture(int tex){ texture = tex; }

	void SetPosition(cVector3df p){ position = p;  }
	void SetAngle(float a){ angle = a; }
	void SetScale(float sc){ scale = sc; }

	float GetAngle(){ return angle; }
	float GetScale(){ return scale; }
	cVector3df GetPosition(){ return position; }


	~CSprite();
};