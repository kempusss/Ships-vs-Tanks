#include <stdio.h>
#include "Engine.h"
#include "Consts.h"

int main()
{
	Engine* game= Engine_create();

	if(game == NULL)
	{
		printf("BRAK GRY\n");
		return 1;
	}

	while(Engine_isActive(game))
	{
		Engine_update(game);
		Engine_draw(game);
	}
	printf("koniec\n");
	Engine_destroy(game);
	return 0;
}