#include "MapManager.h"
#include "Consts.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ERR_PRN printf("LOADING MAP FAILED\n");

typedef enum
{
	WATER_TILE,
	LAND_TILE
} TileType;

MapManager* MapManager_create()
{
	MapManager* manager = malloc(sizeof(MapManager));

	for(int y= 0; y < MAP_HEIGHT; ++y)
	{
		for(int x = 0; x< MAP_WIDTH; ++x)
		{
			for(int i =0;i < 4; ++i)
			{
				manager->cellSpriteId[y][x][i] = -1;
			}
		}
	}

	return manager;
}
void MapManager_loadFromFile(MapManager* manager, const char* filePath)
{
	FILE* data = fopen(filePath, "r");
	if(data == NULL)
	{
		printf("map file path is incorrect\n");
		return;
	}

	for(int y =0;y<MAP_HEIGHT; ++y)
	{
		for(int x= 0; x < MAP_WIDTH; ++x)
		{
			int val;
			if(!fscanf(data, "%d", &val))
			{
				ERR_PRN
				exit(1);
			}
			printf("%d ", val);

			for(int i= 0;i < 4; ++i)
				manager->cellType[y][x] =val;
		}
		printf("\n");
	}

	if(!fscanf(data, "%d %d", &manager->startCellPosition.x, &manager->startCellPosition.y))
	{
		ERR_PRN
		exit(1);
	}

	if(!fscanf(data, "%f", &manager->startRotation))
	{
		ERR_PRN
		exit(1);
	}
	manager->startRotation *= 90;
	if(!fscanf(data, "%d %d", &manager->endCellPosition.x, &manager->endCellPosition.y))
	{
		ERR_PRN
		exit(1);
	}

	fclose(data);
}

static int getCellType(MapManager* manager, int x, int y)
{
	if( x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT)
		return LAND_TILE;
	else
		return manager->cellType[y][x];
}
static void setTile(sfSprite* sprite, int leftID, int cornerID, int topID, int myId, TextureManager* textMan)
{
	int textureId = LAND_MIDDLE;
	
	if(leftID == WATER_TILE && topID == WATER_TILE)	
		textureId = LAND_TOP_LEFT;
	
	else if(leftID == WATER_TILE)
		textureId = LAND_LEFT;
	
	else if(topID == WATER_TILE)
		textureId = LAND_TOP;

	else if(cornerID == WATER_TILE)
		textureId = LAND_TOP_CORNER;


	sfTexture* texture = TextureManager_getTexture(textMan, textureId);
	sfSprite_setTexture(sprite,texture, sfTrue);

	int textureAmount = sfTexture_getSize(texture).x / (MAP_TILE_SIZE);
	int textureVersion = rand() % textureAmount;
	sfIntRect texturePosition = {textureVersion * MAP_TILE_SIZE, myId * MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE};
	sfSprite_setTextureRect(sprite, texturePosition);	
}


void MapManager_createMapTiles(MapManager* manager, SpriteManager* spriteMan, TextureManager* textMan)
{

	const int dX[] = {-1,-1, 0, 1, 1, 1, 0,-1,}; //distance to neighbour positions
	const int dY[] = {0, -1,-1,-1, 0, 1, 1, 1,};
	const int tileDX[] = {0,1,1,0};//ofsset of sprite position
	const int tileDY[] = {0,0,1,1};

	for(int y = 0;y<MAP_HEIGHT; ++y)
	{
		for(int x = 0;x < MAP_WIDTH; ++x)
		{
			for(int i= 0;i<4; ++i)
			{
				SpriteNode* node = SpriteManager_createNode(spriteMan);
				manager->cellSpriteId[y][x][i] = node->id;

				if(manager->cellType[y][x] == WATER_TILE)
				{
					sfSprite_setTexture(node->data, TextureManager_getTexture(textMan, WATER), sfTrue);
				}
				
				else
				{
					int temp[3];
					for(int j = 0;j<3; ++j)
					{
						temp[j] = getCellType(manager, x + dX[(2*i+j)%8], y + dY[(2*i+j)%8]);
					}

					setTile(node->data, temp[0], temp[1], temp[2], i, textMan);	
				}
				sfVector2f pos ={x*TILE_SIZE + tileDX[i]* TILE_SIZE/2, y*TILE_SIZE + tileDY[i] * TILE_SIZE/2};
				sfSprite_setPosition(node->data, pos);
			}

		}
	}
}

void MapManager_createPathDirections(MapManager* manager)
{
	for(int y = 0;y<MAP_HEIGHT; ++y)
	{
		for(int x= 0; x < MAP_WIDTH; ++x)
		{
			manager->distanceToEnd[y][x] = -1;
			manager->pathDirection[y][x] = -1.f;
		}
		puts("\n");
	}
	const int dX[] = {-1,1,0,0};
	const int dY[] = {0,0,-1,1};
	const float angle[] = {270.f, 90.f, 0.f, 180.f};

	typedef struct _Node
	{
		int x;
		int y;
		int distance;
		struct _Node* next;
	}Node;

	Node* queue = malloc(sizeof(Node));
	int queueSize = 1;
	queue->x = manager->endCellPosition.x;
	queue->y = manager->endCellPosition.y;
	queue->distance = 0;

	Node* queueEnd = queue;

	while(queueSize > 0)
	{
		int x= queue->x;
		int y= queue->y;
		int d= queue->distance;

		for(int i = 0;i<4; ++i)
		{
			int nX = x + dX[i];
			int nY = y + dY[i];

			if( getCellType(manager, nX, nY) == WATER_TILE && manager->distanceToEnd[nY][nX] == -1)
			{
				manager->distanceToEnd[nY][nX] = d+1;
				manager->pathDirection[nY][nX] = angle[i];

				queueEnd->next = malloc(sizeof(Node));
				queueEnd = queueEnd->next;
				queueEnd->x = nX;
				queueEnd->y = nY;
				queueEnd->distance = d+1;
				++queueSize;

			}
		}

		Node* temp = queue->next;
		free(queue);
		queue = temp;
		--queueSize;
	}



	for(int y = 0;y<MAP_HEIGHT; ++y)
	{
		for(int x= 0; x < MAP_WIDTH; ++x)
			printf("%.0f ", manager->pathDirection[y][x]);
		puts("\n");
	}
}



float MapManager_getRotation(MapManager* manager, int x, int y)
{
	int cellType = getCellType(manager, x, y);
	if(cellType ==  LAND_TILE)
		return manager->startRotation;
	else
		return manager->pathDirection[y][x];
}
void MapManager_clearMap(MapManager* manager, SpriteManager* spriteMan)
{		
	for(int y = 0;y <MAP_HEIGHT; ++y)
	{
		for(int x= 0; x < MAP_WIDTH; ++x)
		{
			for(int i = 0; i < 4; ++i)
			{
				SpriteManager_destroyNode(spriteMan, manager->cellSpriteId[y][x][i]);
				manager->cellSpriteId[y][x][i] = -1;
			}
		}
	}	
}


void MapManager_destroy(MapManager* manager, SpriteManager* spriteMan)
{	
	if(spriteMan != NULL)
		MapManager_clearMap(manager, spriteMan);
	free(manager);
}

int toMapCoordinates(float pixels)
{
	pixels -= TILE_SIZE/2;
	return round(pixels/TILE_SIZE);
}
float toPixels(int mapCordinate)
{
	return mapCordinate * TILE_SIZE + MAP_TILE_SIZE;
}
