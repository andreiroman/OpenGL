#ifndef __H_ENTITY
#define __H_ENTITY

#include "Vector3d.h"

class CSprite;

class CEntity
{
protected:

	CSprite *gfx;
public:
	//values for collision check
	float	radius;
	float	width, height;

	bool	hasCollision;

	virtual void Init() = 0;
	virtual void Update(float dt) = 0;
	virtual void OnCollision(CEntity * other);
	
	cVector3df  GetPosition();
};

#endif