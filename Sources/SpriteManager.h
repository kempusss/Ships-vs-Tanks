#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include<SFML/Graphics.h>

typedef struct _SpriteNode
{
	int id;
	sfSprite* data;
	struct _SpriteNode *next, *prev;
} SpriteNode;

typedef struct _SpriteManager
{
	SpriteNode *listBegin, *listLast;
	int idCounter;
} SpriteManager;

SpriteManager* SpriteManager_create();
SpriteNode* SpriteManager_createNode(SpriteManager* manager);

SpriteNode* SpriteManager_getNode(SpriteManager* manager, int id);

void SpriteManager_draw(SpriteManager* manager, sfRenderWindow* window);

void SpriteManager_destroyNode(SpriteManager* manager, int id);
void SpriteManager_destroy(SpriteManager*);
void SpriteManager_destroyAllNodes(SpriteManager* manager);

void centerSpriteOrigin(sfSprite * text);

#endif