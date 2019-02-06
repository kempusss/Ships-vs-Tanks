#include "ProjectileManager.h"
#include "Consts.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

ProjectileManager* ProjectileManager_create()
{
	ProjectileManager* manager = malloc(sizeof(ProjectileManager));
	manager->listBegin = NULL;
	manager->listLast = NULL;
	return manager;
}

ProjectileNode* ProjectileManager_createNode(ProjectileManager* manager, SpriteManager* spriteMan, TextureManager* texMan, int type)
{
	ProjectileNode* newNode = malloc(sizeof(ProjectileNode));
	newNode->next = NULL;

	SpriteNode* spriteNode = SpriteManager_createNode(spriteMan);

	newNode->sprite = spriteNode->data;
	newNode->spriteId = spriteNode->id;
	newNode->dmg = 1000;
	newNode->targetX = 0;
	newNode->targetY = 0;
	newNode->targetId = -1;
	newNode->speed = 500.f;
	newNode->onTarget = false;

	sfSprite_setTexture(newNode->sprite, TextureManager_getTexture(texMan, PROJECTILES), sfTrue);
	sfIntRect texturePosition = {0,type*PROJECTILE_SIZE, PROJECTILE_SIZE, PROJECTILE_SIZE};
	sfSprite_setTextureRect(newNode->sprite, texturePosition);
	centerSpriteOrigin(newNode->sprite);

	sfVector2f scale = {PROJECTILE_SCALE, PROJECTILE_SCALE};
	sfSprite_setScale(newNode->sprite, scale);

	sfVector2f position = {0, 0};
	sfSprite_setPosition(newNode->sprite, position);

	if(manager->listBegin == NULL)
	{
		manager->listLast = newNode;
		manager->listBegin = newNode;
		newNode->prev = NULL;
	}
	else
	{
		manager->listLast->next = newNode;
		newNode->prev = manager->listLast;
		manager->listLast = newNode;
	}

	return newNode;
}


static void updateNode(ProjectileNode* node, float deltaTime, EnemyManager* enemyMan )
{
	EnemyNode* target = EnemyManager_getNode(enemyMan, node->targetId);
	if(target != NULL)
	{
		sfVector2f position = sfSprite_getPosition(target->sprite);
		node->targetX = position.x;
		node->targetY = position.y;
	}

	sfVector2f currentPos = sfSprite_getPosition(node->sprite);
	float dX = node->targetX - currentPos.x;
	float dY = node->targetY - currentPos.y;

	
	if(dX*dX + dY*dY < PROJECTILE_DESTROY_DISTANCE*PROJECTILE_DESTROY_DISTANCE)
	{
		node->onTarget= true;
	}
	else
	{
		float angleToTarget = atan2(dY, dX);

		sfVector2f offset ={node->speed * cos(angleToTarget) * deltaTime, node->speed * sin(angleToTarget) * deltaTime};
		sfSprite_move(node->sprite, offset);

		sfSprite_setRotation(node->sprite, angleToTarget/M_PI * 180.f-90.f);
	}
}

void ProjectileManager_update(ProjectileManager* manager, float deltaTime, EnemyManager* enemyMan)
{
	ProjectileNode* node  = manager->listBegin;
	while(node != NULL)
	{
		updateNode(node, deltaTime, enemyMan);
		node = node->next;
	}
}

static void destroyNode(ProjectileManager* manager, ProjectileNode* node, SpriteManager* spriteMan)
{
	if(node->prev == NULL)
	{
		manager->listBegin = node->next;
	}
	else
	{
		node->prev->next = node->next;
	}

	if(node->next == NULL)
	{
		manager->listLast = node->prev;
	}
	else
	{
		node->next->prev = node->prev;
	}

	SpriteManager_destroyNode(spriteMan, node->spriteId);
	free(node);
}
void ProjectileManager_destroyNodesOnTarget(ProjectileManager* manager, SpriteManager* spriteMan, EnemyManager* enemyMan)
{
	ProjectileNode* node  = manager->listBegin;
	while(node != NULL)
	{
		ProjectileNode* next = node->next;
		if(node->onTarget)
		{
			EnemyNode* target= EnemyManager_getNode(enemyMan, node->targetId);
			if(target != NULL)
			{
				target->currentHp -= node->dmg;
				printf("DEALING DMG %d to %d\n", node->dmg, node->targetId);
			}
			destroyNode(manager, node, spriteMan);
		}
		
		node = next;
	}
}

void ProjectileManager_destroyAllNodes(ProjectileManager* manager, SpriteManager* spriteMan)
{
	printf("destroing all Enemies\n");
	ProjectileNode* node = manager->listBegin;

	while(node != NULL)
	{
		ProjectileNode* nex = node->next;
		if(spriteMan != NULL)
			SpriteManager_destroyNode(spriteMan, node->spriteId);
		free(node);
		node = nex;
	}
	manager->listBegin = NULL;
	manager->listLast = NULL;
}
void ProjectileManager_destroy(ProjectileManager* manager, SpriteManager* spriteMan)
{
	printf("destroying Projectile manager\n");
	ProjectileManager_destroyAllNodes(manager, spriteMan);
	free(manager);
}