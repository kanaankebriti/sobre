#include "sobre_engine.h"

// game over!
void kill_sdl(SDL_Renderer* renderer, SDL_Window* main_window)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(main_window);
	SDL_Quit();
}
