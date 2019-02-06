#include "Engine.h"
#include "Consts.h"
#include "MainMenu.h"
#include "Game.h"
#include <stdlib.h>
#include <stdio.h>

#include <SFML/Graphics.h>
#include <SFML/Window.h>


Engine* Engine_create()
{

	printf("Initliazing engine\n");
	srand(1111);
	Engine* eng = malloc(sizeof(Engine));

	//init states
	eng->state = NONE;
	eng->nextState = MAIN_MENU;	
	eng->sceneInfo = NULL;

  	/* Create the main window */
 	sfVideoMode mode = {
   		WINDOW_WIDTH,
    	WINDOW_HEIGHT,
    	32
  	};
  	sfContextSettings settings ={0,0,8,2,0,0,sfFalse};
  	eng->windowsSettings = settings;
 	eng->window = sfRenderWindow_create(mode, "piraci", sfClose, &(eng->windowsSettings));
 	//eng->window = sfRenderWindow_create(mode, "piraci", sfClose, NULL);
  	if (!eng->window)
  	{
  		printf("window creation error\n");
  		return NULL; 
  	}
  	sfRenderWindow_setFramerateLimit(eng->window, FRAME_RATE);
  	sfRenderWindow_setKeyRepeatEnabled(eng->window, false);

  	//createFont
  	eng->font = sfFont_createFromFile(FONT_PATH);
  	if(!eng->font)
  	{
  		printf("cannot load font\n");
  		return NULL;
  	}

  	//create eManagers
  	eng->textureManager = TextureManager_create();
  	eng->spriteManager = SpriteManager_create();
  	eng->textManager = TextManager_create();
  	eng->mapManager = MapManager_create();
  	eng->enemyManager = EnemyManager_create();
  	eng->turretManager = TurretManager_create();
  	eng->projectileManager = ProjectileManager_create();
  	eng->waveManager = WaveManager_create();

  	//create Clock
  	eng->clock= sfClock_create();


  	eng->wasRightMouseButtonPressed = 0;
  	eng->wasLeftMouseButtonPressed = 0;
  	eng->isRightMouseButtonRelased = 0;
  	eng->isLeftMouseButtonRelased = 0;

  	eng->isActive = 1;
	return eng;
}


void Engine_initState(Engine* engine, State stateID)
{	
	free(engine->sceneInfo);

	TextManager_destroyAllNodes(engine->textManager);
	SpriteManager_destroyAllNodes(engine->spriteManager);
	SpriteManager_destroyAllNodes(engine->spriteManager);
	TextManager_destroyAllNodes(engine->textManager);
	EnemyManager_destroyAllNodes(engine->enemyManager, engine->spriteManager);
	TurretManager_destroyAllNodes(engine->turretManager, engine->spriteManager);
	ProjectileManager_destroyAllNodes(engine->projectileManager, engine->spriteManager);
	MapManager_clearMap(engine->mapManager, engine->spriteManager);

	switch(stateID)
	{
		case MAIN_MENU:
			MainMenu_init(engine);
			break;
		case GAME:
			Game_init(engine);
			break;
		default:
			printf("THERE IS NO STATE %d", stateID);
	}
	engine->state = stateID;
}


static void checkMouseButtons(Engine* engine)
{
	//printf("ASDS\n");
    if(!sfMouse_isButtonPressed(sfMouseRight) && engine->wasRightMouseButtonPressed)
    {
    	engine->isRightMouseButtonRelased = 1;
    }
    else
    	engine->isRightMouseButtonRelased = 0;

     if(!sfMouse_isButtonPressed(sfMouseLeft) && engine->wasLeftMouseButtonPressed)
    {	
    	engine->isLeftMouseButtonRelased = 1;
    }
    else
    	engine->isLeftMouseButtonRelased = 0;

    engine->wasLeftMouseButtonPressed = sfMouse_isButtonPressed(sfMouseLeft);
    engine->wasRightMouseButtonPressed = sfMouse_isButtonPressed(sfMouseRight);
}


void Engine_update(Engine* engine)
{

	if(engine->nextState != NONE)
	{
		Engine_initState(engine, engine->nextState);
		engine->nextState = NONE;
	}
	if(!sfRenderWindow_isOpen(engine->window))
	{
		engine->isActive = 0;
		return;
	}
	/* Process events */
	sfEvent event;
    while (sfRenderWindow_pollEvent(engine->window, & event))
    {
      /* Close window : exit */
      if (event.type == sfEvtClosed)
        sfRenderWindow_close(engine->window);
    }

    checkMouseButtons(engine);

    float  deltaTime = sfTime_asSeconds(sfClock_getElapsedTime(engine->clock));
    sfClock_restart(engine->clock);
    switch(engine->state)
    {
    	case MAIN_MENU:
    		MainMenu_update(engine);
    		break;
    	case GAME:
    		Game_update(engine, deltaTime);
    		break;
    	default:
    		printf("Unknown state %d\n", engine->state);
    		break;
    }

}


int Engine_isActive(Engine* engine)
{
	return engine->isActive;
}
void Engine_destroy(Engine* engine)
{
	sfRenderWindow_destroy(engine->window);
	sfFont_destroy(engine->font);
	sfClock_destroy(engine->clock);

	SpriteManager_destroyAllNodes(engine->spriteManager);

	EnemyManager_destroy(engine->enemyManager, NULL);
	TurretManager_destroy(engine->turretManager, NULL);
	ProjectileManager_destroy(engine->projectileManager, NULL);
	MapManager_destroy(engine->mapManager, NULL);
	TextureManager_destroy(engine->textureManager);
	SpriteManager_destroy(engine->spriteManager);
	TextManager_destroy(engine->textManager);
	WaveManager_destroy(engine->waveManager);

	free(engine->sceneInfo);

	free(engine);
}

void Engine_draw(Engine* engine)
{
	sfRenderWindow_clear(engine->window, sfBlack);

	//draw all the stuff here
	SpriteManager_draw(engine->spriteManager, engine->window);	
	TextManager_draw(engine->textManager, engine->window);

	sfRenderWindow_display(engine->window);
}


void Engine_changeState(Engine* engine, State state)
{
	engine->nextState = state;
}