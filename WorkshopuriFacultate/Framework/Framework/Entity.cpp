#include "Entity.h"
#include "Sprite.h"

void CEntity::Init()
{

}

void CEntity::OnCollision(CEntity * other)
{

}

cVector3df CEntity::GetPosition()
{
	if (gfx != 0)
	{
		return gfx->GetPosition();
	}
	return cVector3df(0.0f, 0.0f, 0.0f); 
}