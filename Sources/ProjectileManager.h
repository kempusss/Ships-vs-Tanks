#ifndef PROJECTILE_MANAGER_H
#define PROJECTILE_MANAGER_H

#include "SpriteManager.h"
#include "TextureManager.h"
#include "EnemyManager.h"
#include <stdbool.h>


typedef struct _ProjectileNode
{
	int speed;
	int dmg;
	int targetId;
	float targetX;
	float targetY;
	bool onTarget;

	sfSprite* sprite;
	int spriteId;
	struct _ProjectileNode *next, *prev;
} ProjectileNode;

typedef struct _ProjectileManager
{
	ProjectileNode *listBegin, *listLast;
} ProjectileManager;

ProjectileManager* ProjectileManager_create();
ProjectileNode* ProjectileManager_createNode(ProjectileManager* manager, SpriteManager* spriteMan, TextureManager* texMan, int type);

void ProjectileManager_update(ProjectileManager* manager, float deltaTime, EnemyManager* enemyMan);

void ProjectileManager_destroyNodesOnTarget(ProjectileManager* manager, SpriteManager* spriteMan, EnemyManager* enemyMan);
void ProjectileManager_destroyAllNodes(ProjectileManager* manager, SpriteManager* spriteMan);
void ProjectileManager_destroy(ProjectileManager*manager, SpriteManager* spriteMan);
#endif