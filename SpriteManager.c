#include "SpriteManager.h"
#include<stdlib.h>
#include<stdio.h>


SpriteManager* SpriteManager_create()
{
	SpriteManager* manager = malloc(sizeof(SpriteManager));
	manager->listBegin = NULL;
	manager->listLast = NULL;
	manager->idCounter = 0;
	return manager;
}

SpriteNode*  SpriteManager_createNode(SpriteManager* manager)
{
	SpriteNode* newNode = malloc(sizeof(SpriteNode));
	newNode->data = sfSprite_create();
	newNode->next = NULL;
	newNode->id = manager->idCounter++;

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

SpriteNode* SpriteManager_getNode(SpriteManager* manager, int id)
{
	SpriteNode* node = manager->listBegin;

	while(node!= NULL)
	{
		if(node->id == id)
		{
			printf("finded sprite with id = %d\n", id );
			return node;
		}
		node = node->next;
	}
	printf("There is no sprite with id = %d\n", id);
	return NULL;
}

SpriteNode*  SpriteManager_getNodeContaining(SpriteManager*manager, int x, int y)
{
	SpriteNode* node = manager->listBegin;

	while(node!= NULL)
	{
		sfFloatRect bound = sfSprite_getGlobalBounds(node->data);
		if(sfFloatRect_contains(&bound, x,y))
		{
			printf("finded sprite on position: %d %d\n", x, y );
			return node;
		}
		node = node->next;
	}
	printf("There is no sprite on position: %d %d\n", x,y);
	return NULL;

}

void SpriteManager_draw(SpriteManager* manager, sfRenderWindow* window)
{
	SpriteNode* node = manager->listBegin;

	while(node != NULL)
	{
		sfRenderWindow_drawSprite(window, node->data, NULL);
		node = node->next;
	}
}

void SpriteManager_destroyNode(SpriteManager* manager, int id)
{
	printf("destroying sprite with id = %d\n", id);
	SpriteNode* node = SpriteManager_getNode(manager, id);
	if(node == NULL)
	{
		printf("Nothing to delete\n");
		return;
	}

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

	sfSprite_destroy(node->data);
	free(node);
}

void SpriteManager_destroyAllNodes(SpriteManager* manager)
{
	printf("destroing all sprites\n");
	SpriteNode* node = manager->listBegin;

	while(node != NULL)
	{
		SpriteNode* nex = node->next;
		sfSprite_destroy(node->data);
		free(node);
		node = nex;
	}
	manager->listBegin = NULL;
	manager->listLast = NULL;
	manager->idCounter = 0;
}
void SpriteManager_destroy(SpriteManager* manager)
{
	printf("destroying sprite manager\n");
	SpriteManager_destroyAllNodes(manager);
	free(manager);
}

void centerSpriteOrigin(sfSprite * text)
{
	sfFloatRect rect= sfSprite_getLocalBounds(text);
	sfVector2f center = {rect.width/2, rect.height/2};
	sfSprite_setOrigin(text, center);
}