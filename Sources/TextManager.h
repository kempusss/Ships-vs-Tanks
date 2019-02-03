#ifndef TEXT_MANAGER_H
#define TEXT_MANAGER_H

#include<SFML/Graphics.h>

typedef struct _TextNode
{
	int id;
	sfText* data;
	struct _TextNode *next, *prev;
} TextNode;

typedef struct _TextManager
{
	TextNode *listBegin, *listLast;
	int idCounter;
} TextManager;

TextManager* TextManager_create();
TextNode* TextManager_createNode(TextManager* manager);	

TextNode* TextManager_getNode(TextManager* manager, int id);
TextNode* TextManager_getNodeContaining(TextManager*manager, int x, int y);


void TextManager_draw(TextManager* manager, sfRenderWindow* window);

void TextManager_destroyNode(TextManager* manager, int id);
void TextManager_destroy(TextManager*);
void TextManager_destroyAllNodes(TextManager* manager);

void centerTextOrigin(sfText* text);

#endif