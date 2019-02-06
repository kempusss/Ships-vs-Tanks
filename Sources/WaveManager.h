#ifndef WAVE_MANAGER_H
#define WAVE_MANAGER_H


#include "MapManager.h"
#include "SpriteManager.h"
#include "TextureManager.h"
#include "EnemyManager.h"

typedef enum _LvlStatus
{
	PLAYING,
	WIN,
	LOST,
	LVL_STATUS_COUNT
} LvlStatus;

typedef struct _WaveManager
{
	int shipPassesLeft;
	float timeToSpawn;

	int wavesAmount;
	int currentWave;
	int currentWaveStage;

	int** waves;
	int* waveSize;

	int currentLvlStatus;
	int currentGold;
	int aliveShips;
}WaveManager;

WaveManager* WaveManager_create();
void WaveManager_loadFromFile(WaveManager* manager, const char* filePath);

void WaveManager_update(WaveManager* manager, float deltaTime, MapManager* mapMan, EnemyManager* enemyMan, SpriteManager* spriteMan, TextureManager* textureMan);

void WaveManager_destroy(WaveManager* manager);


#endif