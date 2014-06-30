#include "Background.h"
#include "SpriteManager.h"
#include "Sprite.h"

void CBackground::Update(float dt)
{
	cVector3df pos = gfx->GetPosition();
	pos.y -= dt  * 0.5f;
	gfx->SetPosition(pos);
}

CBackground::CBackground()
{

}
CBackground::~CBackground()
{

}

void CBackground::Init()
{
	gfx = CSpriteManager::Get()->AddSprite("Background.png");
	cVector3df pos(-2.0f,-2.0f,-100.0f);
	gfx->SetPosition(pos);
	gfx->SetScale(20.0f);

	hasCollision = false;
}