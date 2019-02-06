#include "MainMenu.h"
#include "Engine.h"
#include "TextureManager.h"
#include "SpriteManager.h"
#include "TextManager.h"
#include "Consts.h"

#include <stdlib.h>
#include <stdio.h>
typedef enum
{
	BG_SPRITE,
	PLAY_BUTTON,
	EXIT_BUTTON,
	OBJECTS_COUNT	
} ObjectsId;

void MainMenu_update(Engine* engine)
{
	if(engine->isLeftMouseButtonRelased)
	{
		sfVector2i mousePosition = sfMouse_getPositionRenderWindow(engine->window);

		TextNode* node = TextManager_getNodeContaining(engine->textManager, mousePosition.x, mousePosition.y);
		if(node != NULL)
		{
			int id = node->id;
			printf("clicked on text with id = %d\n", id);
			if(id == engine->sceneInfo[PLAY_BUTTON])
			{
				Engine_changeState(engine, GAME);
			}
			else if(id == engine->sceneInfo[EXIT_BUTTON])
			{
				engine->isActive = 0;
			}	
		}
	}
}
void MainMenu_init(Engine* engine)
{

	TextureManager_loadTexture(engine->textureManager, BACKGROUND, "Assets/background.png");
	
	engine->sceneInfo = malloc(OBJECTS_COUNT * sizeof(int));

	SpriteNode* bgSpriteNode = SpriteManager_createNode(engine->spriteManager);
	sfSprite_setTexture(bgSpriteNode->data, 
		TextureManager_getTexture(engine->textureManager, BACKGROUND),
		sfTrue);
	engine->sceneInfo[BG_SPRITE] = bgSpriteNode->id;

	/*create play button*/
	TextNode* playButton = TextManager_createNode(engine->textManager);
	sfText_setFont(playButton->data, engine->font);
	sfText_setString(playButton->data, "PLAY");
	centerTextOrigin(playButton->data);

	sfVector2f pos = {WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f - 200.f};
	sfText_setPosition(playButton->data, pos);

	engine->sceneInfo[PLAY_BUTTON] = playButton->id;

	/*create exit Button*/
	TextNode* exitButton = TextManager_createNode(engine->textManager);
	sfText_setFont(exitButton->data, engine->font);
	sfText_setString(exitButton->data, "EXIT");
	centerTextOrigin(exitButton->data);

	pos.x=WINDOW_WIDTH/2.f;
	pos.y=WINDOW_HEIGHT/2.f + 200.f;
	sfText_setPosition(exitButton->data, pos);

	engine->sceneInfo[EXIT_BUTTON] = exitButton->id;
}