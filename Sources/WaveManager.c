#include "WaveManager.h"
#include <stdlib.h>
#include <stdio.h>

#define ERR_PRN printf("LOAD WAVES FAILED\n");

WaveManager* WaveManager_create()
{
	WaveManager* manager = malloc(sizeof(WaveManager));
	manager->shipPassesLeft = -1;
	manager->timeToSpawn = 0.f;
	manager->waves = NULL;
	manager->currentWave = manager->currentWaveStage = -1;
	manager->currentLvlStatus = LVL_STATUS_COUNT;
	manager->currentGold = 0;
	manager->aliveShips = 0;
	manager->wavesAmount = 0;

	return manager;

}
void WaveManager_loadFromFile(WaveManager* manager, const char* filePath)
{
	FILE* data = fopen(filePath, "r");
	if(data == NULL)
	{
		printf("wave file path is incorrect\n");
		return;
	}

	if(!fscanf(data, "%d", &manager->shipPassesLeft))
	{
		ERR_PRN
		exit(0);
	}
	if(!fscanf(data, "%d", &manager->currentGold))
	{
		ERR_PRN
		exit(0);
	}
	if(!fscanf(data, "%d", &manager->wavesAmount))
	{
		ERR_PRN
		exit(0);
	}

	manager->waves = malloc(sizeof(int*) * manager->wavesAmount);
	manager->waveSize = malloc(sizeof(int) * manager->wavesAmount);

	for(int i= 0; i < manager->wavesAmount; ++i)
	{
		if(!fscanf(data, "%d", &manager->waveSize[i]))
		{
			ERR_PRN
			exit(0);
		}
		manager->waves[i] = malloc(sizeof(int) * manager->waveSize[i]);
		for(int j = 0;j < manager->waveSize[i]; ++j)
		{
			if(!fscanf(data, "%d", &manager->waves[i][j]))
			{
				ERR_PRN
				exit(0);
			}
		}
	}

	manager->currentWave = manager->currentWaveStage = 0;
	manager->timeToSpawn = 0.f;
	manager->currentLvlStatus = PLAYING;
	manager->aliveShips = 0;

}

void WaveManager_update(WaveManager* manager, float deltaTime, MapManager* mapMan, EnemyManager* enemyMan, SpriteManager* spriteMan, TextureManager* textureMan)
{
	manager->timeToSpawn -= deltaTime;

	if(manager->timeToSpawn <= 0 && manager->currentWave < manager->wavesAmount)
	{
		int i = manager->currentWave;
		int j = manager->currentWaveStage;
		EnemyManager_createNode(enemyMan, spriteMan, textureMan, mapMan, manager->waves[i][j]);
		manager->aliveShips++;

		++j;
		if(j == manager->waveSize[i])
		{
			j = 0;
			i++;
			manager->timeToSpawn = WAVE_SPAWN_WAIT;
		}
		else
			manager->timeToSpawn = SHIP_SPAWN_WAIT;

		manager->currentWave = i;
		manager->currentWaveStage = j;
	}

	int kills = EnemyManager_destroyDeadEnemies(enemyMan, spriteMan);
	int passes = EnemyManager_destroyOnPosition(enemyMan, mapMan->endCellPosition, spriteMan);

	manager->aliveShips -= kills + passes;
	manager->currentGold += kills * KILL_GOLD;
	manager->shipPassesLeft -= passes;

	if(manager->shipPassesLeft <= 0)
	{
		manager->currentLvlStatus = LOST;
		return;
	}
	else if(manager->currentWave == manager->wavesAmount && manager->aliveShips == 0)
	{
		manager->currentLvlStatus = WIN;
		return;
	}

}

void WaveManager_destroy(WaveManager* manager)
{
	for(int i= 0; i < manager->wavesAmount; ++i)
	{
		free(manager->waves[i]);
	}
	free(manager->waves);
	free(manager->waveSize);
	free(manager);
}
