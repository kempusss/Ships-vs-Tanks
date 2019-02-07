#ifndef TURRET_MANAGER_H
#define TURRET_MANAGER_H

#include "SpriteManager.h"
#include "TextureManager.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"


typedef struct _TurretNode
{
	int id;
	float range;
	int dmg;
	float reloadTime;
	float timeToShot;
	int value;
	int lvl;

	sfSprite* sprite;
	int spriteId;
	int type;
	struct _TurretNode *next, *prev;
} TurretNode;

typedef struct _TurretManager
{
	TurretNode *listBegin, *listLast;
	int idCounter;
} TurretManager;

TurretManager* TurretManager_create();
TurretNode* TurretManager_createNode(TurretManager* manager, SpriteManager* spriteMan, TextureManager* texMan, int type);

TurretNode* TurretManager_getNode(TurretManager* manager, int id);
TurretNode* TurretManager_getNodeContaining(TurretManager*manager, int x, int y);

void TurretManager_update(TurretManager* manager, float deltaTime, EnemyManager* enemyMan, ProjectileManager* projectileMan, SpriteManager* spriteMan, TextureManager* textureMan);

void TurretManager_destroyNode(TurretManager* manager, int id,SpriteManager* spriteMan);
void TurretManager_destroyAllNodes(TurretManager* manager, SpriteManager* spriteMan);
void TurretManager_destroy(TurretManager*manager, SpriteManager* spriteMan);

int getTurretCost(int type, int lvl);
void upgradeTurret(TurretNode* node);
#endif