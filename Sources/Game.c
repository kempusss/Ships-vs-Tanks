#include "Game.h"
#include "SpriteManager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define SHIP_PASS_TEXT "HP: "
#define GOLD_TEXT "GOLD: "

typedef enum _Action
{
	ACTION_BUILD1,
	ACTION_BUILD2,
	ACTION_BUILD3,
	ACTION_DELETE,
	ACTION_UPGRADE,
	ACTION_CHECK,
	ACTION_COUNT
} Action;

typedef enum _CheckTypes
{
	CHECK_NONE,
	CHECK_ENEMY,
	CHECK_TURRET,
	CHECK_COUNT
} CheckTypes;

typedef enum _Data
{
	TIME_MULTIPLIER,	
	PAUSE_TIME_SPRITE_ID,
	NORMAL_TIME_SPRITE_ID,
	FAST_TIME_SPRITE_ID,
	SHIP_PASSES_LEFT_TEXT_ID,
	GOLD_TEXT_ID,
	RETURN_BUTTON_ID,
	GAME_OVER_INITIALIZED,

	BUILD1_ID,
	BUILD2_ID,
	BUILD3_ID,
	DELETE_ID,
	UPGRADE_ID,
	CHECK_ID,
	CURRENT_ACTION,

	CURRENT_CHECK,
	CHECK_TEXT1_ID,
	CHECK_TEXT2_ID,
	CHECK_SPRITE1_ID,
	CHECK_SPRITE2_ID,
	CHECK_SPRITE3_ID,
	CHECK_SPRITE4_ID,

	DATA_COUNT
}Data;

static void updateText(TextNode* node, int value)
{
	char valueText[5];
	sprintf(valueText, "%d", value);

	sfText_setString(node->data, valueText);
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

	SpriteNode* spriteNode = SpriteManager_createNode(engine->spriteManager);
	sfSprite_setTexture(spriteNode->data, TextureManager_getTexture(engine->textureManager, SHORT_TEXT_BACKGROUND), sfTrue);
	position.y -= 95.f;
	centerSpriteOrigin(spriteNode->data);
	sfSprite_setPosition(spriteNode->data, position);

	spriteNode = SpriteManager_createNode(engine->spriteManager);
	sfSprite_setTexture(spriteNode->data, TextureManager_getTexture(engine->textureManager, LONG_TEXT_BACKGROUND), sfTrue);
	position.y += 100.f;
	centerSpriteOrigin(spriteNode->data);
	sfSprite_setPosition(spriteNode->data, position);
}

static void updateTimeColor(Engine* engine)
{
	int timeMult = engine->sceneInfo[TIME_MULTIPLIER];

	for(int i= 0;i<3; ++i)
	{
		SpriteNode* node = SpriteManager_getNode(engine->spriteManager, engine->sceneInfo[i + PAUSE_TIME_SPRITE_ID]);
		if(i == timeMult)
			sfSprite_setColor(node->data, sfBlue);
		else
			sfSprite_setColor(node->data, sfCyan);
	}
}
void updateActionButtons(Engine* engine)
{
	for(int i= 0;i<ACTION_COUNT; ++i)
	{
		SpriteNode* node = SpriteManager_getNode(engine->spriteManager, engine->sceneInfo[BUILD1_ID + i]);
		if(i == engine->sceneInfo[CURRENT_ACTION])
		{
			sfColor grey = {190,190,190,255};
			sfSprite_setColor(node->data, grey);
		}
		else
			sfSprite_setColor(node->data, sfWhite);
	}
}
static void manageButtons(Engine* engine)
{
	sfVector2i mousePosition = sfMouse_getPositionRenderWindow(engine->window);

	SpriteNode* node = SpriteManager_getNodeContaining(engine->spriteManager, mousePosition.x, mousePosition.y);
	if(node != NULL)
	{
		int id = node->id;
		printf("ID: %d\n", id);
		if(id == engine->sceneInfo[NORMAL_TIME_SPRITE_ID])
		{
			engine->sceneInfo[TIME_MULTIPLIER] = 1;
			updateTimeColor(engine);
		}
		else if(id == engine->sceneInfo[FAST_TIME_SPRITE_ID])
		{
			engine->sceneInfo[TIME_MULTIPLIER] = 2;
			updateTimeColor(engine);
		}
		else if(id == engine->sceneInfo[PAUSE_TIME_SPRITE_ID])
		{
			engine->sceneInfo[TIME_MULTIPLIER] = 0;
			updateTimeColor(engine);
		}
		else if(id == engine->sceneInfo[RETURN_BUTTON_ID])
		{
			Engine_changeState(engine, MAIN_MENU);
		}
		else if(id >= engine->sceneInfo[BUILD1_ID] && id < engine->sceneInfo[BUILD1_ID]+ ACTION_COUNT) // all action buttons where created one after onther, so they have contignous id 
		{
			engine->sceneInfo[CURRENT_ACTION] = id - engine->sceneInfo[BUILD1_ID];
			printf("new action : %d\n", engine->sceneInfo[CURRENT_ACTION]);
			updateActionButtons(engine);
		}	
	}
}
static void manageBuildingTurrets(Engine* engine)
{
	int turretType = engine->sceneInfo[CURRENT_ACTION];
	if(engine->waveManager->currentGold >= getTurretCost(turretType, 1))
	{
		sfVector2i mousePosition = sfMouse_getPositionRenderWindow(engine->window);

		const int mapX = toMapCoordinates(mousePosition.x);
		const int mapY = toMapCoordinates(mousePosition.y);

		if(MapManager_isLand(engine->mapManager, mapX, mapY) && TurretManager_getNodeContaining(engine->turretManager, mousePosition.x, mousePosition.y) == NULL)
		{					
			printf("BUILDING turret\n");
			engine->waveManager->currentGold -= getTurretCost(turretType,1);

			TurretNode* node = TurretManager_createNode(engine->turretManager, engine->spriteManager, engine->textureManager,turretType);
			sfVector2f turretPosition = {toPixels(mapX), toPixels(mapY)};
			sfSprite_setPosition(node->sprite, turretPosition);
		}
		else
		{
			printf("there is already turret on this field\n");
		}
	}
		
	else
	{
		printf("not enough gold\n");
	}
}

static void destroyEnemyCheck(Engine* engine)
{
	SpriteManager_destroyNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE1_ID]);
	SpriteManager_destroyNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE2_ID]);
	SpriteManager_destroyNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE3_ID]);

	TextManager_destroyNode(engine->textManager, engine->sceneInfo[CHECK_TEXT2_ID]);
	TextManager_destroyNode(engine->textManager, engine->sceneInfo[CHECK_TEXT1_ID]);

	engine->sceneInfo[CURRENT_CHECK] = CHECK_NONE;
}

static void destroyTurretCheck(Engine* engine)
{
	SpriteManager_destroyNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE1_ID]);
	SpriteManager_destroyNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE2_ID]);
	SpriteManager_destroyNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE3_ID]);
	SpriteManager_destroyNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE4_ID]);

	TextManager_destroyNode(engine->textManager, engine->sceneInfo[CHECK_TEXT2_ID]);
	TextManager_destroyNode(engine->textManager, engine->sceneInfo[CHECK_TEXT1_ID]);	

	engine->sceneInfo[CURRENT_CHECK] = CHECK_NONE;
}

static void initTurretCheck(Engine* engine)
{
	for(int i = 0;i<4; ++i)	
		engine->sceneInfo[CHECK_SPRITE1_ID + i] = SpriteManager_createNode(engine->spriteManager) ->id;
	for(int i= 0;i<2; ++i)
		engine->sceneInfo[CHECK_TEXT1_ID + i] = TextManager_createNode(engine->textManager) ->id;
}

static void initEnemyCheck(Engine* engine)
{
	for(int i = 0;i<3; ++i)	
		engine->sceneInfo[CHECK_SPRITE1_ID + i] = SpriteManager_createNode(engine->spriteManager) ->id;
	for(int i= 0;i<2; ++i)
		engine->sceneInfo[CHECK_TEXT1_ID + i] = TextManager_createNode(engine->textManager) ->id;
}
static void createTurretCheck(Engine* engine, TurretNode* turretNode)
{
	SpriteNode* spriteNode = SpriteManager_getNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE1_ID]);
	spriteNode->layer =1;
	sfSprite_setTexture(spriteNode->data, sfSprite_getTexture(turretNode->sprite), sfTrue);
	sfSprite_setTextureRect(spriteNode->data, sfSprite_getTextureRect(turretNode->sprite));
	sfVector2f position = {WINDOW_WIDTH - 90.f, 340.f};
	sfSprite_setPosition(spriteNode->data, position);
	sfSprite_setRotation(spriteNode->data, 0.f);

	/*turret range */
	spriteNode = SpriteManager_getNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE4_ID]);
	sfSprite_setTexture(spriteNode->data, TextureManager_getTexture(engine->textureManager, TURRET_RANGE), sfTrue);
	sfVector2f scale= {turretNode->range / 512.f,turretNode->range / 512.f};
	sfSprite_setScale(spriteNode->data, scale);
	sfColor color = {227,240,40,100};
	sfSprite_setColor(spriteNode->data, color);
	centerSpriteOrigin(spriteNode->data);
	sfSprite_setPosition(spriteNode->data, sfSprite_getPosition(turretNode->sprite));
	engine->sceneInfo[CURRENT_CHECK] = CHECK_TURRET;

	/* turret dmg*/
	spriteNode = SpriteManager_getNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE2_ID]);
	spriteNode->layer = 1;
	sfSprite_setTexture(spriteNode->data, TextureManager_getTexture(engine->textureManager, CHECK_ICONS), sfTrue);
	sfIntRect texturePosition = {150,0,50,50};
	sfSprite_setTextureRect(spriteNode->data, texturePosition);
	position.x -= 30.f;
	position.y += 60.f;
	sfSprite_setPosition(spriteNode->data, position);

	TextNode* textNode = TextManager_getNode(engine->textManager, engine->sceneInfo[CHECK_TEXT1_ID]);
	textNode->layer = 1;
	sfText_setFont(textNode->data, engine->font);
	position.x += 80.f;
	position.y += 10.f;
	sfText_setPosition(textNode->data, position);
	sfText_setCharacterSize(textNode->data, 40);
	updateText(textNode, turretNode->dmg);

	/* turret reload */

	spriteNode = SpriteManager_getNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE3_ID]);
	spriteNode->layer = 1;
	sfSprite_setTexture(spriteNode->data, TextureManager_getTexture(engine->textureManager, CHECK_ICONS), sfTrue);
	texturePosition.left = 100.f;
	sfSprite_setTextureRect(spriteNode->data, texturePosition);
	position.x -= 80.f;
	position.y += 40.f;
	sfSprite_setPosition(spriteNode->data, position);

	textNode = TextManager_getNode(engine->textManager, engine->sceneInfo[CHECK_TEXT2_ID]);
	textNode->layer = 1;
	sfText_setFont(textNode->data, engine->font);
	position.x += 80.f;
	position.y += 10.f;
	sfText_setPosition(textNode->data, position);
	sfText_setCharacterSize(textNode->data, 40);
	updateText(textNode, 110 - turretNode->reloadTime *100);
}

static void createEnemyCheck(Engine* engine, EnemyNode* enemyNode)
{
	printf("setting evry\n");
	SpriteNode* spriteNode = SpriteManager_getNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE1_ID]);
	spriteNode->layer =1;
	sfSprite_setTexture(spriteNode->data, sfSprite_getTexture(enemyNode->sprite), sfTrue);
	sfSprite_setTextureRect(spriteNode->data, sfSprite_getTextureRect(enemyNode->sprite));
	sfSprite_setRotation(spriteNode->data, -90.f);

	sfVector2f position = {WINDOW_WIDTH - 120.f, 400.f};
	sfSprite_setPosition(spriteNode->data, position);

	/*ship hp */
	spriteNode = SpriteManager_getNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE2_ID]);
	spriteNode->layer = 1;
	sfSprite_setTexture(spriteNode->data, TextureManager_getTexture(engine->textureManager, CHECK_ICONS), sfTrue);
	sfIntRect texturePosition = {50,0,50,50};
	sfSprite_setTextureRect(spriteNode->data, texturePosition);
	sfSprite_setPosition(spriteNode->data, position);

	TextNode* textNode = TextManager_getNode(engine->textManager, engine->sceneInfo[CHECK_TEXT1_ID]);
	textNode->layer = 1;
	sfText_setFont(textNode->data, engine->font);
	position.x += 80.f;
	position.y += 10.f;
	sfText_setPosition(textNode->data, position);
	sfText_setCharacterSize(textNode->data, 40);
	updateText(textNode, enemyNode->currentHp);

	/* ship speed */

	spriteNode = SpriteManager_getNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE3_ID]);
	spriteNode->layer = 1;
	sfSprite_setTexture(spriteNode->data, TextureManager_getTexture(engine->textureManager, CHECK_ICONS), sfTrue);
	texturePosition.left = 0.f;
	sfSprite_setTextureRect(spriteNode->data, texturePosition);
	position.x -= 80.f;
	position.y += 40.f;
	sfSprite_setPosition(spriteNode->data, position);

	textNode = TextManager_getNode(engine->textManager, engine->sceneInfo[CHECK_TEXT2_ID]);
	textNode->layer = 1;
	sfText_setFont(textNode->data, engine->font);
	position.x += 80.f;
	position.y += 10.f;
	sfText_setPosition(textNode->data, position);
	sfText_setCharacterSize(textNode->data, 40);
	updateText(textNode, enemyNode->speed);

	engine->sceneInfo[CURRENT_CHECK] = CHECK_ENEMY;
}

static void manageCheckDisplay(Engine* engine)
{
	sfVector2i mousePosition = sfMouse_getPositionRenderWindow(engine->window);
	TurretNode* turretNode = TurretManager_getNodeContaining(engine->turretManager, mousePosition.x, mousePosition.y);
	if(turretNode != NULL)
	{
		if(engine->sceneInfo[CURRENT_CHECK] == CHECK_ENEMY)		
			engine->sceneInfo[CHECK_SPRITE4_ID] = SpriteManager_createNode(engine->spriteManager)->id;
		
		
		if(engine->sceneInfo[CURRENT_CHECK] == CHECK_NONE)
			initTurretCheck(engine);
		
		createTurretCheck(engine, turretNode);
		return;
	}

	EnemyNode* enemyNode = EnemyManager_getNodeContaining(engine->enemyManager, mousePosition.x, mousePosition.y);
	if(enemyNode != NULL)
	{
		printf("%d %d %d %d\n", engine->sceneInfo[CHECK_SPRITE1_ID],engine->sceneInfo[CHECK_SPRITE2_ID],engine->sceneInfo[CHECK_SPRITE3_ID],engine->sceneInfo[CHECK_SPRITE4_ID]);
		if(engine->sceneInfo[CURRENT_CHECK] == CHECK_TURRET)		
			SpriteManager_destroyNode(engine->spriteManager, engine->sceneInfo[CHECK_SPRITE4_ID]);
		
		if(engine->sceneInfo[CURRENT_CHECK] == CHECK_NONE)
			initEnemyCheck(engine);
		printf("Create enemy check\n");
		createEnemyCheck(engine, enemyNode);
	}
}

static void manageDeletingTurrets(Engine* engine)
{
	sfVector2i mousePosition = sfMouse_getPositionRenderWindow(engine->window);

	TurretNode* node = TurretManager_getNodeContaining(engine->turretManager, mousePosition.x, mousePosition.y);
	if(node != NULL)
	{
		engine->waveManager->currentGold += node->value /2.f;
		TurretManager_destroyNode(engine->turretManager, node->id, engine->spriteManager);
	}
}
static void manageUpgradingTurrets(Engine* engine)
{
	sfVector2i mousePosition = sfMouse_getPositionRenderWindow(engine->window);
	TurretNode* node = TurretManager_getNodeContaining(engine->turretManager, mousePosition.x, mousePosition.y);
	if(node != NULL)
	{
		int type = node->type;
		if(node->lvl != MAX_LVL && engine->waveManager->currentGold >= getTurretCost(type,node->lvl+1))
		{
			engine->waveManager->currentGold -= getTurretCost(type, node->lvl+1);
			upgradeTurret(node);
		}
	}
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
	

	if(engine->isLeftMouseButtonReleased)
	{
		manageButtons(engine);	
		if(engine->sceneInfo[CURRENT_ACTION] == ACTION_DELETE)
			manageDeletingTurrets(engine);

		if (engine->sceneInfo[CURRENT_ACTION] <= ACTION_BUILD3 && engine->sceneInfo[CURRENT_ACTION] >= ACTION_BUILD1)
			manageBuildingTurrets(engine);

		if(engine->sceneInfo[CURRENT_ACTION] == ACTION_CHECK)
			manageCheckDisplay(engine);
		else
		{
			if(engine->sceneInfo[CURRENT_CHECK] == CHECK_ENEMY)
				destroyEnemyCheck(engine);
			else if(engine->sceneInfo[CURRENT_CHECK] == CHECK_TURRET)
				destroyTurretCheck(engine);
		}
		if(engine->sceneInfo[CURRENT_ACTION] == ACTION_UPGRADE)
			manageUpgradingTurrets(engine);


	}

	float frameTime = deltaTime * engine->sceneInfo[TIME_MULTIPLIER];
	
	EnemyManager_update(engine->enemyManager, frameTime, engine->mapManager);
	TurretManager_update(engine->turretManager, frameTime, engine->enemyManager, engine->projectileManager, engine->spriteManager, engine->textureManager);
	ProjectileManager_update(engine->projectileManager, frameTime, engine->enemyManager);
	ProjectileManager_destroyNodesOnTarget(engine->projectileManager, engine->spriteManager, engine->enemyManager);

	WaveManager_update(engine->waveManager, frameTime, engine->mapManager, engine->enemyManager, engine->spriteManager, engine->textureManager);

	updateText(TextManager_getNode(engine->textManager, engine->sceneInfo[SHIP_PASSES_LEFT_TEXT_ID]), engine->waveManager->shipPassesLeft);
	updateText(TextManager_getNode(engine->textManager, engine->sceneInfo[GOLD_TEXT_ID]), engine->waveManager->currentGold);
}

static void setupTimeButtons(Engine* engine)
{	
	engine->sceneInfo[TIME_MULTIPLIER] = 1;

	/*normal time mult*/
	SpriteNode* node = SpriteManager_createNode(engine->spriteManager);
	node->layer = 1;
	engine->sceneInfo[NORMAL_TIME_SPRITE_ID] = node->id;

	sfSprite_setTexture(node->data,TextureManager_getTexture(engine->textureManager, TIME_ICONS), sfTrue);
	sfIntRect texturePosition = {0,0,50,50};
	sfSprite_setTextureRect(node->data, texturePosition);

	sfVector2f position = {WINDOW_WIDTH - 137.f, 60.f};
	sfSprite_setPosition(node->data, position);

	/*fast time mult*/
	node = SpriteManager_createNode(engine->spriteManager);
	node->layer = 1;
	engine->sceneInfo[FAST_TIME_SPRITE_ID] = node->id;

	sfSprite_setTexture(node->data,TextureManager_getTexture(engine->textureManager, TIME_ICONS), sfTrue);
	texturePosition.left = 50;
	sfSprite_setTextureRect(node->data, texturePosition);

	position.x += 45;
	sfSprite_setPosition(node->data, position);

	/*pause time mult*/
	node = SpriteManager_createNode(engine->spriteManager);
	node->layer = 1;
	engine->sceneInfo[PAUSE_TIME_SPRITE_ID] = node->id;

	sfSprite_setTexture(node->data,TextureManager_getTexture(engine->textureManager, TIME_ICONS), sfTrue);
	texturePosition.left = 100;
	sfSprite_setTextureRect(node->data, texturePosition);

	position.x += 45;
	sfSprite_setPosition(node->data, position);

	updateTimeColor(engine);
}

static void setupReturnButton(Engine* engine)
{
	SpriteNode* returnNode = SpriteManager_createNode(engine->spriteManager);
	returnNode->layer = 1;
	engine->sceneInfo[RETURN_BUTTON_ID] = returnNode->id;

	sfSprite_setTexture(returnNode->data, TextureManager_getTexture(engine->textureManager, HOME_ICON), sfTrue);
	sfVector2f position = {WINDOW_WIDTH- 50.f, 0.f};
	sfSprite_setPosition(returnNode->data, position);

	sfColor green = {31,220,24,255};
	sfSprite_setColor(returnNode->data, green);
}

static void setupGoldText(Engine* engine)
{
	/* create gold text*/	
	TextNode* textNode = TextManager_createNode(engine->textManager);
	textNode->layer = 1;
	engine->sceneInfo[GOLD_TEXT_ID] = textNode->id;

	sfText_setFont(textNode->data, engine->font);
	sfText_setCharacterSize(textNode->data, 40);
	sfVector2f position = {WINDOW_WIDTH-38.f, 210.f};
	sfText_setPosition(textNode->data, position);
	sfText_setColor(textNode->data, sfYellow);

	/*create gold icon*/

	SpriteNode* spriteNode = SpriteManager_createNode(engine->spriteManager);
	spriteNode->layer = 1;
	sfSprite_setTexture(spriteNode->data, TextureManager_getTexture(engine->textureManager, GOLD_ICON), sfTrue);
	position.x -= 85.f;
	position.y -= 11.f;
	sfSprite_setPosition(spriteNode->data, position);
}

static void setupHpText(Engine* engine)
{
	/* create hp text*/
	TextNode* textNode = TextManager_createNode(engine->textManager);
	textNode->layer = 1;
	engine->sceneInfo[SHIP_PASSES_LEFT_TEXT_ID] = textNode->id;

	sfText_setFont(textNode->data, engine->font);
	sfText_setCharacterSize(textNode->data, 40);
	sfVector2f position = {WINDOW_WIDTH-38.f, 150.f};
	sfText_setPosition(textNode->data, position);
	sfText_setColor(textNode->data, sfRed);

	SpriteNode* spriteNode = SpriteManager_createNode(engine->spriteManager);
	spriteNode->layer = 1;
	sfSprite_setTexture(spriteNode->data, TextureManager_getTexture(engine->textureManager, HP_ICON), sfTrue);
	position.x -= 85.f;
	position.y -= 11.f;
	sfSprite_setPosition(spriteNode->data, position);
}

static void setupActionButtons(Engine* engine)
{
	for(int i= 0;i<ACTION_COUNT; ++i)
	{
		SpriteNode* node = SpriteManager_createNode(engine->spriteManager);
		node->layer = 1;
		engine->sceneInfo[BUILD1_ID + i] = node->id;

		sfSprite_setTexture(node->data, TextureManager_getTexture(engine->textureManager, ACTION_ICONS), sfTrue);
		sfIntRect texturePosition = {i*50, 0 ,50, 50};
		sfSprite_setTextureRect(node->data, texturePosition);

		sfVector2f position = {WINDOW_WIDTH - 110 + i%2*44, WINDOW_HEIGHT-155 + i/2*50};
		sfSprite_setPosition(node->data, position);
	}


	engine->sceneInfo[CURRENT_ACTION] = ACTION_CHECK;
	updateActionButtons(engine);
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
	TextureManager_loadTexture(engine->textureManager, TIME_ICONS, "Assets/timeIcons.png");
	TextureManager_loadTexture(engine->textureManager, HOME_ICON, "Assets/homeIcon.png");
	TextureManager_loadTexture(engine->textureManager, GOLD_ICON, "Assets/goldIcon.png");
	TextureManager_loadTexture(engine->textureManager, HP_ICON, "Assets/hpIcon.png");
	TextureManager_loadTexture(engine->textureManager, ACTION_ICONS, "Assets/actionIcons.png");
	TextureManager_loadTexture(engine->textureManager, TURRET_RANGE, "Assets/turretRange.png");
	TextureManager_loadTexture(engine->textureManager, CHECK_ICONS, "Assets/checkIcons.png");	
	TextureManager_loadTexture(engine->textureManager, BACKGROUND, "Assets/backgroundGUI.png");
	TextureManager_loadTexture(engine->textureManager, SHORT_TEXT_BACKGROUND, "Assets/shortTextBackground.png");
	TextureManager_loadTexture(engine->textureManager, LONG_TEXT_BACKGROUND, "Assets/longTextBackground.png");

	SpriteNode* background = SpriteManager_createNode(engine->spriteManager);
	sfSprite_setTexture(background->data, TextureManager_getTexture(engine->textureManager, BACKGROUND), sfTrue);
	sfVector2f position = {MAP_WIDTH * TILE_SIZE, 0};
	sfSprite_setPosition(background->data, position);
	background->layer = 1;
	background->canContain = 0;

	MapManager_loadFromFile(engine->mapManager, engine->mapPath);
	MapManager_createMapTiles(engine->mapManager, engine->spriteManager, engine->textureManager);
	MapManager_createPathDirections(engine->mapManager);
	WaveManager_loadFromFile(engine->waveManager, engine->wavePath);

	engine->sceneInfo = malloc(DATA_COUNT * sizeof(int));

	/*create time text*/
	setupTimeButtons(engine);
	setupReturnButton(engine);
	setupGoldText(engine);	
	setupHpText(engine);
	setupActionButtons(engine);

	engine->sceneInfo[GAME_OVER_INITIALIZED] = 0;
	engine->sceneInfo[CURRENT_CHECK] = CHECK_NONE;
}