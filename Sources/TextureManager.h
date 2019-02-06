#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include<SFML/Graphics.h>

typedef enum 
{
	WATER,
	LAND_TOP_LEFT,
	LAND_TOP,
	LAND_TOP_CORNER,
	LAND_LEFT,
	LAND_MIDDLE,
	SHIPS,
	BACKGROUND,
	TANKS,
	PROJECTILES,
	TIME_ICONS,
	HOME_ICON,
	GOLD_ICON,
	HP_ICON,
	ACTION_ICONS,
	TEXTURE_COUNT
}TextureNames;

typedef struct _TextureManager
{
	sfTexture* textures[TEXTURE_COUNT];
} TextureManager;


TextureManager* TextureManager_create();
void TextureManager_loadTexture(TextureManager* manager, TextureNames name, const char* path);
void TextureManager_destroy(TextureManager* manager);
sfTexture* TextureManager_getTexture(TextureManager* manager, TextureNames name);

#endif