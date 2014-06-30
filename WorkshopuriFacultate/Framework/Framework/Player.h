#ifndef __H_PLAYER
#define __H_PLAYER

#include "Entity.h"

class CPlayer : public CEntity
{
private:
	float player_move_speed;
public:
	CPlayer();
	~CPlayer();

	virtual void Init();
	void Update(float dt);
};

#endif