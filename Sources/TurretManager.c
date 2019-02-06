#include "TurretManager.h"
#include "Consts.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static const int dmg[3] = {15,24,32};
static const float reloadTime[3] = {0.5f, 0.75f,1.f};
static const float range[3] = {200.f,300.f,400.f};
static const int cost[3] = {20,25,30};


TurretManager* TurretManager_create()
{
	TurretManager* manager = malloc(sizeof(TurretManager));
	manager->listBegin = NULL;
	manager->listLast = NULL;
	manager->idCounter = 0;
	return manager;
}

TurretNode* TurretManager_createNode(TurretManager* manager, SpriteManager* spriteMan, TextureManager* texMan, int type)
{
	TurretNode* newNode = malloc(sizeof(TurretNode));
	newNode->next = NULL;
	newNode->id = manager->idCounter++;


	SpriteNode* spriteNode = SpriteManager_createNode(spriteMan);

	newNode->sprite = spriteNode->data;
	newNode->spriteId = spriteNode->id;
	newNode->range = range[type];
	newNode->dmg = dmg[type];
	newNode->reloadTime = newNode->timeToShot = reloadTime[type];
	newNode->type = type;

	sfSprite_setTexture(newNode->sprite, TextureManager_getTexture(texMan, TANKS), sfTrue);

	sfIntRect texturePosition = {0,type*TILE_SIZE, TILE_SIZE, TILE_SIZE};
	sfSprite_setTextureRect(newNode->sprite, texturePosition);
	centerSpriteOrigin(newNode->sprite);

	sfVector2f scale = {TURRET_SCALE, TURRET_SCALE};
	sfSprite_setScale(newNode->sprite, scale);

	sfVector2f position = {3.5f*TILE_SIZE, 4.5f*TILE_SIZE};
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

TurretNode* TurretManager_getNode(TurretManager* manager, int id)
{
	TurretNode* node = manager->listBegin;

	while(node!= NULL)
	{
		if(node->id == id)
		{
			printf("finded Turret with id = %d\n", id );
			return node;
		}
		node = node->next;
	}
	printf("There is no Turret with id = %d\n", id);
	return NULL;
}

TurretNode*  TurretManager_getNodeContaining(TurretManager*manager, int x, int y)
{
	TurretNode* node = manager->listBegin;

	while(node!= NULL)
	{
		sfFloatRect bound = sfSprite_getGlobalBounds(node->sprite);
		if(sfFloatRect_contains(&bound, x,y))
		{
			printf("finded Turret on position: %d %d\n", x, y );
			return node;
		}
		node = node->next;
	}
	printf("There is no Turret on position: %d %d\n", x,y);
	return NULL;

}

static void updateNode(TurretNode* turret, float deltaTime, EnemyManager* enemyMan, ProjectileManager* projectileMan, SpriteManager* spriteMan, TextureManager* textureMan)
{
	
	sfVector2f position = sfSprite_getPosition(turret->sprite);
	EnemyNode* enemy = EnemyManager_getClosestInRadius(enemyMan, position.x, position.y, turret->range);
	
	if(enemy != NULL)
	{
		sfVector2f enemyPosition = sfSprite_getPosition(enemy->sprite);
		sfVector2f turretPosition = sfSprite_getPosition(turret->sprite);
		float dX = enemyPosition.x - turretPosition.x;
		float dY = enemyPosition.y - turretPosition.y;

		sfSprite_setRotation(turret->sprite, atan2(dY, dX) / M_PI * 180 - 90.f);
	}

	turret->timeToShot -= deltaTime;
	if(turret->timeToShot <= 0)
	{
		if(enemy != NULL)
		{
			printf("Shooting\n");
			turret->timeToShot = turret->reloadTime;
			ProjectileNode* projectile = ProjectileManager_createNode(projectileMan, spriteMan, textureMan, turret->type);
			projectile->targetId = enemy->id;
			projectile->dmg = turret->dmg;
			//speed
			sfSprite_setPosition(projectile->sprite, sfSprite_getPosition(turret->sprite));
		}
		else
			turret->timeToShot = 0;
	}
}

void TurretManager_update(TurretManager* manager, float deltaTime, EnemyManager* enemyMan, ProjectileManager* projectileMan, SpriteManager* spriteMan,  TextureManager* textureMan)
{
	TurretNode* node  = manager->listBegin;
	while(node != NULL)
	{
		updateNode(node, deltaTime, enemyMan, projectileMan, spriteMan, textureMan);
		node = node->next;
	}
}

static void destroyNode (TurretManager* manager, TurretNode* node, SpriteManager* spriteMan)
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

void TurretManager_destroyNode(TurretManager* manager, int id, SpriteManager* spriteMan)
{
	printf("destroying Turret with id = %d\n", id);
	TurretNode* node = TurretManager_getNode(manager, id);
	if(node == NULL)
	{
		printf("Nothing to delete\n");
		return;
	}
	destroyNode(manager, node, spriteMan);
}

void TurretManager_destroyAllNodes(TurretManager* manager, SpriteManager* spriteMan)
{
	printf("destroing all Enemies\n");
	TurretNode* node = manager->listBegin;

	while(node != NULL)
	{
		TurretNode* nex = node->next;

		if(spriteMan != NULL)
			SpriteManager_destroyNode(spriteMan, node->spriteId);

		free(node);
		node = nex;
	}
	manager->listBegin = NULL;
	manager->listLast = NULL;
	manager->idCounter = 0;
}
void TurretManager_destroy(TurretManager* manager, SpriteManager* spriteMan)
{
	printf("destroying Turret manager\n");
	TurretManager_destroyAllNodes(manager, spriteMan);
	free(manager);
}


int getTurretCost(int type)
{
	return cost[type];
}