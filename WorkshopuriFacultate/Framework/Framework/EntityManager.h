#ifndef __H_ENTITYMANAGER
#define __H_ENTITYMANAGER
#include <vector>

class CEntity;

class CEntityManager
{
	float prev_update_time;

	std::vector<CEntity*> all_entities;
public:
	CEntityManager();
	~CEntityManager();

	void CheckCollisions();
	void Update();
	void AddEntity(CEntity * newEntity);

	static CEntityManager * Get()
	{
		return &me;
	}
	static CEntityManager me;
};

#endif