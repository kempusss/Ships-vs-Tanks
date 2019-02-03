#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.h>
#include "TextureManager.h"
#include "SpriteManager.h"
#include "TextureManager.h"
#include "TextManager.h"
#include "MapManager.h"
#include "EnemyManager.h"

typedef enum
{
	NONE,
	MAIN_MENU,
	GAME,
	STATE_COUNT
} State;

typedef struct _Engine
{
	int isActive;
	State state;
	State nextState;
	sfRenderWindow* window;

	TextureManager* textureManager;
	SpriteManager* spriteManager;
	TextManager* textManager;


	EnemyManager* enemyManager;
	//TurretManager* turretManager;
	MapManager* mapManager;

	sfFont* font;
	sfClock* clock;

	int* sceneInfo;

} Engine;

Engine* Engine_create();
void Engine_update(Engine* engine);
int Engine_isActive(Engine* engine);
void Engine_destroy(Engine* engine);
void Engine_draw(Engine* engine);
void Engine_changeState(Engine* engine, State state);

#endif