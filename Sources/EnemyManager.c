#include "EnemyManager.h"
#include "Consts.h"
#include <stdio.h>
#include <stdlib.h>

EnemyManager* EnemyManager_create()
{
	EnemyManager* manager = malloc(sizeof(EnemyManager));
	manager->listBegin = NULL;
	manager->listLast = NULL;
	manager->idCounter = 0;
	return manager;
}

EnemyNode*  EnemyManager_createNode(EnemyManager* manager, SpriteManager* spriteMan, TextureManager* texMan, MapManager* mapMan)
{
	EnemyNode* newNode = malloc(sizeof(EnemyNode));
	newNode->next = NULL;
	newNode->id = manager->idCounter++;


	SpriteNode* spriteNode = SpriteManager_createNode(spriteMan);
	if(spriteNode == NULL)
	{
		printf("ERROR, no sprite\n");
		return NULL;
	}

	newNode->sprite = spriteNode->data;
	newNode->spriteId = spriteNode->id;
	newNode->maxHp = newNode->currentHp = 100;
	newNode->speed = 200;

	sfSprite_setTexture(newNode->sprite, TextureManager_getTexture(texMan, SHIP), sfTrue);
	sfIntRect texturePosition = {0,0,TILE_SIZE,TILE_SIZE*2};
	sfSprite_setTextureRect(newNode->sprite, texturePosition);
	centerSpriteOrigin(newNode->sprite);

	sfVector2f scale = {DEFAULT_SCALE, DEFAULT_SCALE};
	sfSprite_setScale(newNode->sprite, scale);

	sfVector2f position = {toPixels(mapMan->startCellPosition.x), toPixels(mapMan->startCellPosition.y)};
	sfSprite_setPosition(newNode->sprite, position);

	sfSprite_setRotation(newNode->sprite, mapMan->startRotation);



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

EnemyNode* EnemyManager_getNode(EnemyManager* manager, int id)
{
	EnemyNode* node = manager->listBegin;

	while(node!= NULL)
	{
		if(node->id == id)
		{
			printf("finded Enemy with id = %d\n", id );
			return node;
		}
		node = node->next;
	}
	printf("There is no Enemy with id = %d\n", id);
	return NULL;
}

EnemyNode*  EnemyManager_getNodeContaining(EnemyManager*manager, int x, int y)
{
	EnemyNode* node = manager->listBegin;

	while(node!= NULL)
	{
		sfFloatRect bound = sfSprite_getGlobalBounds(node->sprite);
		if(sfFloatRect_contains(&bound, x,y))
		{
			printf("finded Enemy on position: %d %d\n", x, y );
			return node;
		}
		node = node->next;
	}
	printf("There is no Enemy on position: %d %d\n", x,y);
	return NULL;

}

static void updatePosition(EnemyNode* node, float rotation, sfVector2f position, float deltaTime)
{
	/*align the position*/
	if(rotation == 0.f || rotation == 180.f)
		position.x = toPixels(toMapCoordinates(position.x));
	else
		position.y = toPixels(toMapCoordinates(position.y));

	/*move the enemy*/
	if(rotation == 0.f)
	{
		position.y += deltaTime * node->speed;
	}
	else if(rotation == 180.f)
	{
		position.y -= deltaTime * node->speed;
	}
	else if(rotation == 90.f)
	{
		position.x -= deltaTime * node->speed;
	}
	else
	{
		position.x += deltaTime * node->speed;
	}

	sfSprite_setRotation(node->sprite, rotation);
	sfSprite_setPosition(node->sprite, position);
}

static void updateNode(EnemyNode* enemy, float deltaTime, MapManager* mapMan)
{
	float hpPercentage = (float)enemy->currentHp / enemy->maxHp;
	sfIntRect textureRect = sfSprite_getTextureRect(enemy->sprite);

	if(hpPercentage < 0.6f && hpPercentage > 0.3f)		
		textureRect.left = TILE_SIZE;

	if(hpPercentage <= 0.3f)
		textureRect.left = TILE_SIZE * 2;
	sfSprite_setTextureRect(enemy->sprite, textureRect);

	sfVector2f position = sfSprite_getPosition(enemy->	sprite);
	float rotation = MapManager_getRotation(mapMan, toMapCoordinates(position.x), toMapCoordinates(position.y));
	updatePosition(enemy, rotation, position, deltaTime);

	enemy->currentHp--;	
}

static void destroyNode (EnemyManager* manager, EnemyNode* node, SpriteManager* spriteMan)
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

void EnemyManager_update(EnemyManager* manager, float deltaTime, MapManager* mapMan)
{
	EnemyNode* node  = manager->listBegin;
	while(node != NULL)
	{
		updateNode(node, deltaTime, mapMan);
		node = node->next;
	}
}

int EnemyManager_destroyOnPosition(EnemyManager* manager, sfVector2i position, SpriteManager* spriteMan)
{
	int finishedAmount = 0;
	EnemyNode* node  = manager->listBegin;
	while(node != NULL)
	{
		sfVector2f spritePositon = sfSprite_getPosition(node->sprite);
		sfVector2i mapPosition = {toMapCoordinates(spritePositon.x), toMapCoordinates(spritePositon.y)};
		if(position.x == mapPosition.x && position.y == mapPosition.y)
		{
			EnemyNode* temp = node->next;
			destroyNode(manager, node, spriteMan);
			node = temp;
			++finishedAmount;
		}
		else
			node = node->next;
	}
	return finishedAmount;
}

int EnemyManager_destroyDeadEnemies(EnemyManager* manager, SpriteManager* spriteMan)
{
	int amount = 0;
	EnemyNode* node  = manager->listBegin;
	while(node != NULL)
	{
		if(node->currentHp <= 0)
		{
			EnemyNode* temp = node->next;
			destroyNode(manager, node, spriteMan);
			node = temp;
			++amount;
		}
		else
			node = node->next;
	}
	return amount;
}

void EnemyManager_destroyNode(EnemyManager* manager, int id, SpriteManager* spriteMan)
{
	printf("destroying Enemy with id = %d\n", id);
	EnemyNode* node = EnemyManager_getNode(manager, id);
	if(node == NULL)
	{
		printf("Nothing to delete\n");
		return;
	}
	destroyNode(manager, node, spriteMan);
}

void EnemyManager_destroyAllNodes(EnemyManager* manager, SpriteManager* spriteMan)
{
	printf("destroing all Enemies\n");
	EnemyNode* node = manager->listBegin;

	while(node != NULL)
	{
		EnemyNode* nex = node->next;
		SpriteManager_destroyNode(spriteMan, node->spriteId);
		free(node);
		node = nex;
	}
	manager->listBegin = NULL;
	manager->listLast = NULL;
	manager->idCounter = 0;
}
void EnemyManager_destroy(EnemyManager* manager, SpriteManager* spriteMan)
{
	printf("destroying Enemy manager\n");
	EnemyManager_destroyAllNodes(manager, spriteMan);
	free(manager);
}