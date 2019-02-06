#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.h>
#include "TextureManager.h"
#include "SpriteManager.h"
#include "TextureManager.h"
#include "TextManager.h"
#include "MapManager.h"
#include "EnemyManager.h"
#include "TurretManager.h"
#include "ProjectileManager.h"
#include "WaveManager.h"

#include <stdbool.h>

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
	TurretManager* turretManager;
	MapManager* mapManager;
	ProjectileManager* projectileManager;
	WaveManager* waveManager;

	sfFont* font;
	sfClock* clock;

	int* sceneInfo;
	sfContextSettings windowsSettings;

	bool isLeftMouseButtonReleased;
	bool isRightMouseButtonReleased;
	bool wasRightMouseButtonPressed;
	bool wasLeftMouseButtonPressed;

} Engine;

Engine* Engine_create();
void Engine_update(Engine* engine);
int Engine_isActive(Engine* engine);
void Engine_destroy(Engine* engine);
void Engine_draw(Engine* engine);
void Engine_changeState(Engine* engine, State state);

#endif