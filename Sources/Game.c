#include "Game.h"
#include "SpriteManager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define TIME_TEXT "Time x " 
#define SHIP_PASS_TEXT "HP: "
#define GOLD_TEXT "GOLD: "

typedef enum _Data
{
	TIME_MULTIPLIER,
	TIME_TEXT_ID,
	SHIP_PASS_LEFT_TEXT_ID,
	GOLD_TEXT_ID,
	RETURN_BUTTON,
	GAME_OVER_INITIALIZED,
	LAST_FRAME_CLICKED,
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
static bool isAnyKeyPressed()
{
	for(int i= -1; i < sfKeyCount; ++i)
		if(sfKeyboard_isKeyPressed(i))
			return true;
	return false;
}

static void createGameOverTexts(Engine* engine, const char* str1, const char* str2)
{
	engine->sceneInfo[GAME_OVER_INITIALIZED] = 1;
	TextNode* node = TextManager_createNode(engine->textManager);
	sfText_setFont(node->data, engine->font);
	sfVector2f position = {WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f};
	sfText_setPosition(node->data, position);
	sfText_setString(node->data, str1);
	centerTextOrigin(node->data);
	sfText_setColor(node->data, sfBlack);
	sfText_setOutlineColor(node->data, sfCyan);
	sfText_setOutlineThickness(node->data, 2);	


	position.y += 100.f;
	node = TextManager_createNode(engine->textManager);
	sfText_setFont(node->data, engine->font);
	sfText_setPosition(node->data, position);
	sfText_setString(node->data, str2);
	centerTextOrigin(node->data);
	sfText_setColor(node->data, sfBlack);			
	sfText_setOutlineColor(node->data, sfCyan);
	sfText_setOutlineThickness(node->data, 2);	
}

void Game_update(Engine* engine, float deltaTime)
{

	if(engine->waveManager->currentLvlStatus == LOST)
	{
		if(!engine->sceneInfo[GAME_OVER_INITIALIZED])
		{
			createGameOverTexts(engine, "YOU LOST :(", "CLICK ANYTHING TO CONTINUE");
		}

		if(sfMouse_isButtonPressed(sfMouseLeft) || sfMouse_isButtonPressed(sfMouseRight) || isAnyKeyPressed())
			Engine_changeState(engine, MAIN_MENU);
		return;	
	}
	
	if(engine->waveManager->currentLvlStatus == WIN)
	{
		if(!engine->sceneInfo[GAME_OVER_INITIALIZED])
		{
			createGameOverTexts(engine, "YOU WIN :)", "CLICK ANYTHING TO CONTINUE");
		}
		
		if(sfMouse_isButtonPressed(sfMouseLeft) || sfMouse_isButtonPressed(sfMouseRight) || isAnyKeyPressed())
			Engine_changeState(engine, MAIN_MENU);
		return;	
	}

	if(engine->isLeftMouseButtonRelased)
	{
		sfVector2i mousePosition = sfMouse_getPositionRenderWindow(engine->window);

		TextNode* node = TextManager_getNodeContaining(engine->textManager, mousePosition.x, mousePosition.y);
		if(node != NULL)
		{
			int id = node->id;
			printf("clicked on text with id = %d\n", id);
			if(id == engine->sceneInfo[TIME_TEXT_ID])
			{
				if(engine->sceneInfo[TIME_MULTIPLIER] != 2)
					engine->sceneInfo[TIME_MULTIPLIER] = 2;
				else
					engine->sceneInfo[TIME_MULTIPLIER] = 1;
				updateText(node, TIME_TEXT, engine->sceneInfo[TIME_MULTIPLIER]);
			}
			else if(id == engine->sceneInfo[RETURN_BUTTON])
			{
				Engine_changeState(engine, MAIN_MENU);
			}	
		}
	}
	float frameTime = deltaTime * engine->sceneInfo[TIME_MULTIPLIER];

	
	EnemyManager_update(engine->enemyManager, frameTime, engine->mapManager);
	TurretManager_update(engine->turretManager, frameTime, engine->enemyManager, engine->projectileManager, engine->spriteManager, engine->textureManager);
	ProjectileManager_update(engine->projectileManager, frameTime, engine->enemyManager);
	ProjectileManager_destroyNodesOnTarget(engine->projectileManager, engine->spriteManager, engine->enemyManager);

	WaveManager_update(engine->waveManager, frameTime, engine->mapManager, engine->enemyManager, engine->spriteManager, engine->textureManager);

	updateText(TextManager_getNode(engine->textManager, engine->sceneInfo[SHIP_PASS_LEFT_TEXT_ID]), SHIP_PASS_TEXT, engine->waveManager->shipPassesLeft);
	updateText(TextManager_getNode(engine->textManager, engine->sceneInfo[GOLD_TEXT_ID]), GOLD_TEXT, engine->waveManager->currentGold);
}

void Game_init(Engine* engine)
{
	TextureManager_loadTexture(engine->textureManager, WATER, "Assets/Tiles/water.png");
	TextureManager_loadTexture(engine->textureManager, LAND_MIDDLE, "Assets/Tiles/landMiddle.png");
	TextureManager_loadTexture(engine->textureManager, LAND_TOP_LEFT, "Assets/Tiles/landTopLeft.png");
	TextureManager_loadTexture(engine->textureManager, LAND_TOP, "Assets/Tiles/landTop.png");
	TextureManager_loadTexture(engine->textureManager, LAND_LEFT, "Assets/Tiles/landLeft.png");
	TextureManager_loadTexture(engine->textureManager, LAND_TOP_CORNER, "Assets/Tiles/landTopCorner.png");
	TextureManager_loadTexture(engine->textureManager, SHIPS, "Assets/ships.png");
	TextureManager_loadTexture(engine->textureManager, TANKS, "Assets/tanks.png");
	TextureManager_loadTexture(engine->textureManager, PROJECTILES, "Assets/projectiles.png");

	
	MapManager_loadFromFile(engine->mapManager, "Maps/map1");
	MapManager_createMapTiles(engine->mapManager, engine->spriteManager, engine->textureManager);
	MapManager_createPathDirections(engine->mapManager);
	WaveManager_loadFromFile(engine->waveManager, "Maps/map1-waves");

	/*temp functions*/
	TurretManager_createNode(engine->turretManager, engine->spriteManager, engine->textureManager);

	engine->sceneInfo = malloc(DATA_COUNT * sizeof(int));

	/*create time text*/
	engine->sceneInfo[TIME_MULTIPLIER] = 1;
	TextNode* node = TextManager_createNode(engine->textManager);
	engine->sceneInfo[TIME_TEXT_ID] = node->id;
	sfText_setFont(node->data, engine->font);
	updateText(node, TIME_TEXT, engine->sceneInfo[TIME_MULTIPLIER]);
	sfVector2f position = {WINDOW_WIDTH - 70.f, WINDOW_HEIGHT - 50.f};
	sfText_setPosition(node->data, position);

	/* create hp text*/
	node = TextManager_createNode(engine->textManager);
	engine->sceneInfo[SHIP_PASS_LEFT_TEXT_ID] = node->id;
	sfText_setFont(node->data, engine->font);
	position.y = 50.f;
	position.x = WINDOW_WIDTH - 70.f;
	sfText_setPosition(node->data, position);

	/* create gold text*/
	node = TextManager_createNode(engine->textManager);
	engine->sceneInfo[GOLD_TEXT_ID] = node->id;
	sfText_setFont(node->data, engine->font);
	position.y = 150.f;
	position.x = WINDOW_WIDTH - 70.f;
	sfText_setPosition(node->data, position);
	sfText_setColor(node->data, sfYellow);
	

	/*create return text*/
	node = TextManager_createNode(engine->textManager);
	engine->sceneInfo[RETURN_BUTTON] = node->id;
	sfText_setFont(node->data, engine->font);
	sfText_setString(node->data, "MENU");
	position.y = 0.f;
	position.x = WINDOW_WIDTH - 100.f;
	sfText_setPosition(node->data, position);

	engine->sceneInfo[GAME_OVER_INITIALIZED] = 0;
}
