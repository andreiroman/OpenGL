#ifndef __H_BACKGROUND
#define __H_BACKGROUND

#include "Entity.h"

class CBackground : public CEntity
{

public:
	CBackground();
	~CBackground();

	virtual void Init();
	void Update(float dt);

};

#endif