#include "TextManager.h"

#include<stdlib.h>
#include<stdio.h>


TextManager* TextManager_create()
{
	TextManager* manager = malloc(sizeof(TextManager));
	manager->listBegin = NULL;
	manager->listLast = NULL;
	manager->idCounter = 0;
	return manager;
}

TextNode*  TextManager_createNode(TextManager* manager)
{
	TextNode* newNode = malloc(sizeof(TextNode));
	newNode->data = sfText_create();
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

TextNode* TextManager_getNode(TextManager* manager, int id)
{
	TextNode* node = manager->listBegin;

	while(node!= NULL)
	{
		if(node->id == id)
		{
			//printf("finded text with id = %d\n",id );
			return node;
		}
		node = node->next;
	}
	printf("There is no text with id = %d\n", id);
	return NULL;
}

TextNode*  TextManager_getNodeContaining(TextManager*manager, int x, int y)
{
	TextNode* node = manager->listBegin;

	while(node!= NULL)
	{
		sfFloatRect bound = sfText_getGlobalBounds(node->data);
		if(sfFloatRect_contains(&bound, x,y))
		{
			printf("finded text on position: %d %d\n", x, y );
			return node;
		}
		node = node->next;
	}
	printf("There is no text on position: %d %d\n", x,y);
	return NULL;

}


void TextManager_draw(TextManager* manager, sfRenderWindow* window)
{
	TextNode* node = manager->listBegin;

	while(node != NULL)
	{
		sfRenderWindow_drawText(window, node->data, NULL);
		node = node->next;
	}
}

void TextManager_destroyNode(TextManager* manager, int id)
{
	printf("destorying text with id = %d\n", id);
	TextNode* node = TextManager_getNode(manager, id);
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

	sfText_destroy(node->data);
	free(node);
}

void TextManager_destroyAllNodes(TextManager* manager)
{
	printf("destroying all texts\n");
	TextNode* node = manager->listBegin;

	while(node != NULL)
	{
		TextNode* nex = node->next;
		sfText_destroy(node->data);
		free(node);
		node = nex;
	}

	manager->listBegin = NULL;
	manager->listLast = NULL;
	manager->idCounter = 0;
}
void TextManager_destroy(TextManager* manager)
{
	printf("destroying text manager\n");
	TextManager_destroyAllNodes(manager);
	free(manager);
}


void centerTextOrigin(sfText * text)
{
	sfFloatRect rect= sfText_getLocalBounds(text);
	sfVector2f center = {rect.width/2, rect.height/2};
	sfText_setOrigin(text, center);
}

