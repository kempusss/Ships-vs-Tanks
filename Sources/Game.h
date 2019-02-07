#ifndef GAME_H
#define GAME_H

#include "Engine.h"
void Game_update(Engine* engine, float deltaTime);
void Game_init(Engine* engine);
void Game_redrawGUI(Engine* engine, sfRenderWindow* window);

#endif