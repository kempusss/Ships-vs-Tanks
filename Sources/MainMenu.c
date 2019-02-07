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
	LVL1_BUTTON,
	LVL2_BUTTON,
	LVL3_BUTTON,
	EXIT_BUTTON,
	OBJECTS_COUNT	
} ObjectsId;

void MainMenu_update(Engine* engine)
{
	if(engine->isLeftMouseButtonReleased)
	{
		sfVector2i mousePosition = sfMouse_getPositionRenderWindow(engine->window);

		SpriteNode* node = SpriteManager_getNodeContaining(engine->spriteManager, mousePosition.x, mousePosition.y);
		if(node != NULL)
		{
			int id = node->id;

			printf("clicked on text with id = %d\n", id);
			if(id == engine->sceneInfo[LVL1_BUTTON])
			{
				engine->mapPath = "Maps/map1";
				engine->wavePath = "Maps/map1-waves";
				Engine_changeState(engine, GAME);
			}
			else if(id == engine->sceneInfo[LVL2_BUTTON])
			{
				engine->mapPath = "Maps/map2";
				engine->wavePath = "Maps/map2-waves";
				Engine_changeState(engine, GAME);
			}
			else if(id == engine->sceneInfo[LVL3_BUTTON])
			{
				engine->mapPath = "Maps/map3";
				engine->wavePath = "Maps/map3-waves";
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
	TextureManager_loadTexture(engine->textureManager, SHORT_TEXT_BACKGROUND, "Assets/shortTextBackground.png");
	
	engine->sceneInfo = malloc(OBJECTS_COUNT * sizeof(int));

	SpriteNode* bgSpriteNode = SpriteManager_createNode(engine->spriteManager);
	sfSprite_setTexture(bgSpriteNode->data, TextureManager_getTexture(engine->textureManager, BACKGROUND), sfTrue);
	bgSpriteNode->canContain = 0;

	/*create LVL1 button*/
	TextNode* playButtonText = TextManager_createNode(engine->textManager);
	sfText_setFont(playButtonText->data, engine->font);
	sfText_setString(playButtonText->data, "LVL1");
	centerTextOrigin(playButtonText->data);
	sfVector2f pos = {WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f - 200.f};
	sfText_setPosition(playButtonText->data, pos);		
	sfText_setColor(playButtonText->data, sfBlack);	
	sfText_setOutlineColor(playButtonText->data, sfCyan);
	sfText_setOutlineThickness(playButtonText->data, 2);	

	SpriteNode* playButtonSprite = SpriteManager_createNode(engine->spriteManager);
	sfSprite_setTexture(playButtonSprite->data, TextureManager_getTexture(engine->textureManager, SHORT_TEXT_BACKGROUND), sfTrue);
	centerSpriteOrigin(playButtonSprite->data);
	pos.y += 9.f;
	sfSprite_setPosition(playButtonSprite->data, pos);
	pos.y -= 9.f;
	engine->sceneInfo[LVL1_BUTTON] = playButtonSprite->id;

	/*create LVL2 button */
	playButtonText = TextManager_createNode(engine->textManager);
	sfText_setFont(playButtonText->data, engine->font);
	sfText_setString(playButtonText->data, "LVL2");
	centerTextOrigin(playButtonText->data);
	pos.y += 100.f;
	sfText_setPosition(playButtonText->data, pos);		
	sfText_setColor(playButtonText->data, sfBlack);	
	sfText_setOutlineColor(playButtonText->data, sfCyan);
	sfText_setOutlineThickness(playButtonText->data, 2);	

	playButtonSprite = SpriteManager_createNode(engine->spriteManager);
	sfSprite_setTexture(playButtonSprite->data, TextureManager_getTexture(engine->textureManager, SHORT_TEXT_BACKGROUND), sfTrue);
	centerSpriteOrigin(playButtonSprite->data);
	pos.y += 9.f;
	sfSprite_setPosition(playButtonSprite->data, pos);
	pos.y -= 9.f;
	engine->sceneInfo[LVL2_BUTTON] = playButtonSprite->id;

	/*create LVL3 button */
	playButtonText = TextManager_createNode(engine->textManager);
	sfText_setFont(playButtonText->data, engine->font);
	sfText_setString(playButtonText->data, "LVL3");
	centerTextOrigin(playButtonText->data);
	pos.y += 100.f;
	sfText_setPosition(playButtonText->data, pos);		
	sfText_setColor(playButtonText->data, sfBlack);	
	sfText_setOutlineColor(playButtonText->data, sfCyan);
	sfText_setOutlineThickness(playButtonText->data, 2);	

	playButtonSprite = SpriteManager_createNode(engine->spriteManager);
	sfSprite_setTexture(playButtonSprite->data, TextureManager_getTexture(engine->textureManager, SHORT_TEXT_BACKGROUND), sfTrue);
	centerSpriteOrigin(playButtonSprite->data);
	pos.y += 9.f;
	sfSprite_setPosition(playButtonSprite->data, pos);
	pos.y -= 9.f;
	engine->sceneInfo[LVL3_BUTTON] = playButtonSprite->id;

	/*create exit Button*/
	TextNode* exitButtonText = TextManager_createNode(engine->textManager);
	sfText_setFont(exitButtonText->data, engine->font);
	sfText_setString(exitButtonText->data, "EXIT");
	centerTextOrigin(exitButtonText->data);
	pos.x=WINDOW_WIDTH/2.f;
	pos.y=WINDOW_HEIGHT/2.f + 200.f;
	sfText_setPosition(exitButtonText->data, pos);	
	sfText_setColor(exitButtonText->data, sfBlack);	
	sfText_setOutlineColor(exitButtonText->data, sfCyan);
	sfText_setOutlineThickness(exitButtonText->data, 2);


	SpriteNode* exitButtonSprite = SpriteManager_createNode(engine->spriteManager);
	sfSprite_setTexture(exitButtonSprite->data, TextureManager_getTexture(engine->textureManager, SHORT_TEXT_BACKGROUND), sfTrue);
	centerSpriteOrigin(exitButtonSprite->data);
	pos.y += 9.f;
	sfSprite_setPosition(exitButtonSprite->data, pos);


	engine->sceneInfo[EXIT_BUTTON] = exitButtonSprite->id;
}