#include "sobre_engine.h"

bool init_sdl(SDL_Renderer** renderer, SDL_Window** window)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
		return false;
	// check vulkan
	*window =
		SDL_CreateWindow
		(
			WIN_TITLE_STR,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WIN_WIDTH,
			WIN_HEIGHT,
			SDL_WINDOW_VULKAN
			//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN
			//SDL_WINDOW_FULLSCREEN_DESKTOP
		);
	// check opengl
	if (!*window)
		*window =
		SDL_CreateWindow
		(
			WIN_TITLE_STR,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WIN_WIDTH,
			WIN_HEIGHT,
			0
			//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL
			//SDL_WINDOW_FULLSCREEN_DESKTOP
		);
	// if none works
	if (!*window)
		return false;
	*renderer =
		SDL_CreateRenderer
		(
			*window,
			-1, //SDL_RENDERER_SOFTWARE
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
		);
	if (!*renderer)
		return false;

	return true;
}
