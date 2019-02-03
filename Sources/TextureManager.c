#include "TextureManager.h"
#include <stdio.h>
#include <stdlib.h>

TextureManager* TextureManager_create()
{
	TextureManager* manager = malloc(sizeof(TextureManager));

	for(int i =0;i<TEXTURE_COUNT;++i)
		manager->textures[i] = NULL;

	return manager;
}


void TextureManager_loadTexture(TextureManager* manager, TextureNames name, const char* path)
{
	if(name < 0 || name >= TEXTURE_COUNT)
	{
		printf("wrong texture name");
		return;
	}
	if(manager->textures[name] != NULL)
	{
		printf("texture was already loaded\n");
		sfTexture_destroy(manager->textures[name]);
	}

	manager->textures[name] = sfTexture_createFromFile(path, NULL);

}

sfTexture* TextureManager_getTexture(TextureManager* manager, TextureNames name)
{
	if(name < 0 || name >= TEXTURE_COUNT)
	{
		printf("wrong texture name");
		return NULL;
	}
	if(manager->textures[name] == NULL)
	{
		printf("texture is not loaded\n");
		return NULL;
	}
	return manager->textures[name];
}

void TextureManager_destroy(TextureManager* manager)
{
	for(int i =0;i <TEXTURE_COUNT; ++i)
	{
		sfTexture_destroy(manager->textures[i]);
	}

	free(manager);
}