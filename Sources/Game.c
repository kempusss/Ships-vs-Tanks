#include "Game.h"
#include "SpriteManager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TIME_TEXT "Time mult: " 
#define SHIP_PASS_TEXT "HP: "

typedef enum _Data
{
	TIME_MULTIPLIER,
	TIME_TEXT_ID,
	SHIP_PASS_LEFT,
	SHIP_PASS_LEFT_TEXT_ID,
	RETURN_BUTTON,
	DATA_COUNT
}Data;

static void updateText(TextNode* node, const char* text, int value)
{
	char buffor[40]={};
	char valueText[5];
	sprintf(valueText, "%d", value);

	strcat(buffor, text);
	strcat(buffor, valueText);
	sfText_setString(node->data, buffor);
	centerTextOrigin(node->data);
}

void Game_update(Engine* engine, float deltaTime)
{

	if(sfMouse_isButtonPressed(sfMouseLeft))
	{
		sfVector2i mousePosition = sfMouse_getPositionRenderWindow(engine->window);

		TextNode* node = TextManager_getNodeContaining(engine->textManager, mousePosition.x, mousePosition.y);
		if(node != NULL)
		{
			int id = node->id;
			printf("clicked on text with id = %d\n", id);
			if(id == engine->sceneInfo[TIME_TEXT_ID])
			{
				if(engine->sceneInfo[TIME_MULTIPLIER] == 2)
					engine->sceneInfo[TIME_MULTIPLIER] = 1;
				else
					engine->sceneInfo[TIME_MULTIPLIER] = 2;
				updateText(node, TIME_TEXT, engine->sceneInfo[TIME_MULTIPLIER]);
			}
			else if(id == engine->sceneInfo[RETURN_BUTTON])
			{
				Engine_changeState(engine, MAIN_MENU);
			}	
		}
	}

	//printf("update %f\n", deltaTime);
	EnemyManager_update(engine->enemyManager, deltaTime * engine->sceneInfo[TIME_MULTIPLIER], engine->mapManager);
	//TurretManager_update();
	EnemyManager_destroyDeadEnemies(engine->enemyManager, engine->spriteManager);
	int change = EnemyManager_destroyOnPosition(engine->enemyManager, engine->mapManager->endCellPosition, engine->spriteManager);
	if(change > 0)
	{
		engine->sceneInfo[SHIP_PASS_LEFT] -= change;
		if(engine->sceneInfo[SHIP_PASS_LEFT] == 0)
		{
			Engine_changeState(engine, MAIN_MENU);
		}
		updateText(TextManager_getNode(engine->textManager, engine->sceneInfo[SHIP_PASS_LEFT_TEXT_ID]), SHIP_PASS_TEXT, engine->sceneInfo[SHIP_PASS_LEFT]);
	}
}

void Game_init(Engine* engine)
{
	TextManager_destroyAllNodes(engine->textManager);
	SpriteManager_destroyAllNodes(engine->spriteManager);

	TextureManager_loadTexture(engine->textureManager, WATER, "Assets/Tiles/water.png");
	TextureManager_loadTexture(engine->textureManager, LAND_MIDDLE, "Assets/Tiles/landMiddle.png");
	TextureManager_loadTexture(engine->textureManager, LAND_TOP_LEFT, "Assets/Tiles/landTopLeft.png");
	TextureManager_loadTexture(engine->textureManager, LAND_TOP, "Assets/Tiles/landTop.png");
	TextureManager_loadTexture(engine->textureManager, LAND_LEFT, "Assets/Tiles/landLeft.png");
	TextureManager_loadTexture(engine->textureManager, LAND_TOP_CORNER, "Assets/Tiles/landTopCorner.png");
	TextureManager_loadTexture(engine->textureManager, SHIP, "Assets/ships.png");
	
	MapManager_loadFromFile(engine->mapManager, "Maps/map1");
	MapManager_createMapTiles(engine->mapManager, engine->spriteManager, engine->textureManager);
	MapManager_createPathDirections(engine->mapManager);
	EnemyManager_createNode(engine->enemyManager, engine->spriteManager, engine->textureManager, engine->mapManager);

	free(engine->sceneInfo);
	engine->sceneInfo = malloc(DATA_COUNT * sizeof(int));

	/*create time text*/
	engine->sceneInfo[TIME_MULTIPLIER] = 1;
	TextNode* node = TextManager_createNode(engine->textManager);
	engine->sceneInfo[TIME_TEXT_ID] = node->id;
	sfText_setFont(node->data, engine->font);
	updateText(node, TIME_TEXT,engine->sceneInfo[TIME_MULTIPLIER]);
	sfVector2f position = {WINDOW_WIDTH - 220.f, WINDOW_HEIGHT - 50.f};
	sfText_setPosition(node->data, position);

	/* create hp text*/
	engine->sceneInfo[SHIP_PASS_LEFT] = 1;
	node = TextManager_createNode(engine->textManager);
	engine->sceneInfo[SHIP_PASS_LEFT_TEXT_ID] = node->id;
	sfText_setFont(node->data, engine->font);
	updateText(node, SHIP_PASS_TEXT, engine->sceneInfo[SHIP_PASS_LEFT]);
	position.y = 50.f;
	position.x = WINDOW_WIDTH - 70.f;
	sfText_setPosition(node->data, position);


	/*create return text*/
	node = TextManager_createNode(engine->textManager);
	engine->sceneInfo[RETURN_BUTTON] = node->id;
	sfText_setFont(node->data, engine->font);
	sfText_setString(node->data, "MENU");
	position.y = 0.f;
	position.x = WINDOW_WIDTH - 100.f;
	sfText_setPosition(node->data, position);
}
