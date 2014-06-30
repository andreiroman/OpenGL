#include "common.h"
#include "Player.h"
#include "SpriteManager.h"
#include "Sprite.h"

CPlayer::CPlayer()
{
	player_move_speed = 2.0f;
}
CPlayer::~CPlayer()
{

}

void CPlayer::Init()
{
	gfx = CSpriteManager::Get()->AddSprite("PlayerRocket.png");
	cVector3df pos(0.0f, 0.0f, -2.0f);
	gfx->SetPosition(pos);

	radius = gfx->GetScale();
	width = gfx->GetScale()* gfx->GetAspectRatio();
	height = gfx->GetScale();
	hasCollision = true;
}

void CPlayer::Update(float dt)
{
	cVector3df pos;
	cVector3df dir(0.0f,0.0f,0.0f);

	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT))
	{
		dir.x = -1.0f;
	}
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		dir.x = 1.0f;;
	}
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP))
	{
		dir.y = 1.0f;
	}
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN))
	{
		dir.y = -1.0f;
	}
	dir.Normalize();

	pos = gfx->GetPosition();
	pos = pos + dir * player_move_speed * dt;

	if (pos.x < SCREEN_LEFT )
	{
		pos.x = SCREEN_LEFT + 0.01f;
	}
	if (pos.x + gfx->GetScale()* gfx->GetAspectRatio()  > SCREEN_RIGHT)
	{
		pos.x = SCREEN_RIGHT - 0.01f - gfx->GetScale() * gfx->GetAspectRatio();
	}
	if (pos.y < SCREEN_BOTTOM)
	{
		pos.y = SCREEN_BOTTOM + 0.01f;
	}
	if (pos.y + gfx->GetScale()  > SCREEN_TOP)
	{
		pos.y = SCREEN_TOP - 0.01f - gfx->GetScale();
	}

	gfx->SetPosition(pos);
}