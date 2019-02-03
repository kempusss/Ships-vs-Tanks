#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H
#include "SpriteManager.h"
#include "TextureManager.h"
#include "MapManager.h"

typedef struct _EnemyNode
{
	int id;
	sfSprite* sprite;
	int spriteId;

	int currentHp;
	int maxHp;
	int speed;

	struct _EnemyNode *next, *prev;
} EnemyNode;

typedef struct _EnemyManager
{
	EnemyNode *listBegin, *listLast;
	int idCounter;

} EnemyManager;

EnemyManager* EnemyManager_create();
EnemyNode* EnemyManager_createNode(EnemyManager* manager, SpriteManager* spriteMan, TextureManager* texMan, MapManager* mapMan);

EnemyNode* EnemyManager_getNode(EnemyManager* manager, int id);

void EnemyManager_update(EnemyManager* manager, float deltaTime, MapManager* mapMan);

int EnemyManager_destroyOnPosition(EnemyManager* manager, sfVector2i endPosition,SpriteManager* spriteMan);
int EnemyManager_destroyDeadEnemies(EnemyManager* manager, SpriteManager* SpriteMan);
void EnemyManager_destroyNode(EnemyManager* manager, int id,SpriteManager* spriteMan);
void EnemyManager_destroyAllNodes(EnemyManager* manager, SpriteManager* spriteMan);
void EnemyManager_destroy(EnemyManager*manager, SpriteManager* spriteMan);

#endif