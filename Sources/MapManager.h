#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include "Consts.h"
#include "SpriteManager.h"
#include "TextureManager.h"
#include <stdbool.h>

typedef struct _MapManager
{
	int cellType[MAP_HEIGHT][MAP_WIDTH];
	int cellSpriteId[MAP_HEIGHT][MAP_WIDTH][4];
	float pathDirection[MAP_HEIGHT][MAP_WIDTH];
	int distanceToEnd[MAP_HEIGHT][MAP_WIDTH];

	sfVector2i startCellPosition;
	sfVector2i endCellPosition;
	float startRotation;

	int maxShipPass;	
} MapManager;

MapManager* MapManager_create();
void MapManager_loadFromFile(MapManager* manager, const char* filePath);
void MapManager_createMapTiles(MapManager* manager, SpriteManager* spriteMan, TextureManager* textMan);
void MapManager_createPathDirections(MapManager* manager); //require map tiles to be corectly placed

bool MapManager_isLand(MapManager* manager, int x, int y);
sfVector2f MapManager_getStartPosition(MapManager* manager);
float MapManager_getRotation(MapManager* manager, int x, int y);

void MapManager_clearMap(MapManager* manager, SpriteManager* spriteMan);
void MapManager_destroy(MapManager* manager, SpriteManager* spriteMan);

int toMapCoordinates(float pixels);
float toPixels(int mapCordinate);

#endif