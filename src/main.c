//#define TIP_SHADOW_FIX_WIDTH 3		// fill the gap!
//#define TIP_SHADOW_FIX_ANG 3		// fill the gap!

#include "sobre_engine.h"
#include <SDL2_gfxPrimitives.h>

#ifdef _WIN32
	#ifdef _DEBUG
		#define _CRTDBG_MAP_ALLOC
		#include <crtdbg.h>
		#ifndef DBG_NEW
			#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
			#define new DBG_NEW
		#endif
	#endif	// _DEBUG

	#ifdef _DEBUG
		#pragma comment(lib, "SDL2d.lib")
	#else
		#pragma comment(lib, "SDL2.lib")
	#endif

	#pragma comment(lib, "SDL2_image.lib")
	#pragma comment(lib, "SDL2_ttf.lib")
	#pragma comment(lib, "SDL2_mixer.lib")
	#pragma comment(lib, "SDL2_gfx.lib")
	#pragma comment(lib, "cjson.lib")
#endif	// _WIN32

// rad to deg approx
inline double rad_to_deg_approx(double rad)
{
	return rad * 57;
}

#ifdef _WIN32
int APIENTRY WinMain
(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
)
#else
int main()
#endif // _WIN32
{
	#ifdef _WIN32
	// not used parameters!
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
	#endif // _WIN32

	SDL_Renderer* renderer;
	SDL_Window* main_window;
	unsigned char game_state = CMD_RUN_MAIN_MENU;

	// init SDL2
	if (!init_sdl(&renderer, &main_window))
	{
		#ifdef _WIN32
		MessageBox(NULL, L"SDL2.dll failure", L"Error", MB_ICONERROR | MB_OK);
		#endif // _WIN32
		return EXIT_FAILURE;
	}

	// init SDL2_ttf
	if (TTF_Init() == -1) {
		#ifdef _WIN32
		MessageBox(NULL, L"SDL2_ttf.dll failure", L"Error", MB_ICONERROR | MB_OK);
		#endif // _WIN32
		return EXIT_FAILURE;
	}

	// Initialize SDL2_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 4096) == -1)
	{
		#ifdef _WIN32
		MessageBox(NULL, L"SDL2_mixer.dll failure", L"Error", MB_ICONERROR | MB_OK);
		#endif // _WIN32
		return EXIT_FAILURE;
	}

	// init random seed
	srand((unsigned int)time(NULL));
	
	while (game_state != CMD_END_GAME)
	{
		switch (game_state)
		{
		case CMD_RUN_MAIN_MENU:
			game_state = run_main_menu(renderer);
			break;
		case CMD_RUN_GAME_1:
			game_state = run_game_1(renderer);
			break;
		case CMD_RUN_FAIL:
			return EXIT_FAILURE;
		}
	}
	/*
	SDL_Surface* img_surface = NULL;
	SDL_Texture* shadow_texture = NULL;
	SDL_Texture* black_light_texture = NULL;
	SDL_Texture* white_light_texture = NULL;
	SDL_Texture* background_texture = NULL;
	SDL_Rect light_rect;

	light_rect.w = 700;
	light_rect.h = 700;

	shadow_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, WIN_WIDTH, WIN_HEIGHT);
	img_surface = IMG_Load("wall.png");
	background_texture = SDL_CreateTextureFromSurface(renderer, img_surface);
	img_surface = IMG_Load("plight-black.png");
	black_light_texture = SDL_CreateTextureFromSurface(renderer, img_surface);
	img_surface = IMG_Load("plight-white.png");
	white_light_texture = SDL_CreateTextureFromSurface(renderer, img_surface);

	// set blends
	SDL_SetTextureBlendMode(shadow_texture, SDL_BLENDMODE_MUL);
	SDL_SetTextureBlendMode(black_light_texture, SDL_BLENDMODE_MUL);
	SDL_SetTextureBlendMode(white_light_texture, SDL_BLENDMODE_BLEND);

	int mouse_pos_x, mouse_pos_y;

	struct point_2d {
		double x;
		double y;
	} typedef point_2d;

	struct cast_circle {
		double x;
		double y;
		double height;
		double radius;
	}typedef cast_circle;

	struct spot_light {
		double x;
		double y;
		double z;
		unsigned int range;
	} typedef spot_light;

	// rectangular shadow e.g. shadow body
	struct shadow_quad {
		Sint16 x1, x2, x3, x4;
		Sint16 y1, y2, y3, y4;
	}typedef shadow_quad;

	spot_light light;
	light.x = 400;
	light.y = 400;
	light.z = 70;
	light.range = 700;

	cast_circle culumn1;
	culumn1.x = 600;
	culumn1.y = 300;
	culumn1.radius = 40;
	culumn1.height = 40;

	shadow_quad column1_shadow_body;

	// shadow body calculations
	double height_diff = culumn1.height - light.z;
	double angle = atan2(light.x - culumn1.x, culumn1.y - light.y) + 1.57079f;
	column1_shadow_body.x1 = culumn1.x + sin(angle) * culumn1.radius;
	column1_shadow_body.y1 = culumn1.y - cos(angle) * culumn1.radius;
	column1_shadow_body.x2 = culumn1.x - sin(angle) * culumn1.radius;
	column1_shadow_body.y2 = culumn1.y + cos(angle) * culumn1.radius;
	column1_shadow_body.x3 = ((culumn1.height * light.x) - (column1_shadow_body.x2 * light.z)) / height_diff;
	column1_shadow_body.y3 = ((culumn1.height * light.y) - (column1_shadow_body.y2 * light.z)) / height_diff;
	column1_shadow_body.x4 = ((culumn1.height * light.x) - (column1_shadow_body.x1 * light.z)) / height_diff;
	column1_shadow_body.y4 = ((culumn1.height * light.y) - (column1_shadow_body.y1 * light.z)) / height_diff;

	// shadow tip calculations
	point_2d shadow_far_edge_center =
	{
		(column1_shadow_body.x4 + column1_shadow_body.x3) / 2,
		(column1_shadow_body.y4 + column1_shadow_body.y3) / 2
	};
	double shadow_tip_radius;

	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderPresent(renderer);
	SDL_Event event;
	while (SDL_WaitEvent(&event))
		switch (event.type)
		{
			// on keyboard keydown
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_w)
				light.z += 1;
			else if (event.key.keysym.sym == SDLK_s)
				light.z -= 1;
			else if (event.key.keysym.sym == SDLK_ESCAPE)
				goto free_up_memory_ad_return;
		case SDL_MOUSEMOTION:
			// retrive mouse relative location
			SDL_GetMouseState(&mouse_pos_x, &mouse_pos_y);
			light.x = mouse_pos_x;
			light.y = mouse_pos_y;

			// cast shadow only in range
			if (sqrt(pow(mouse_pos_x - culumn1.x, 2) + pow(mouse_pos_y - culumn1.y, 2)) > light.range)
			{
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, background_texture, NULL, NULL);
				filledCircleColor(renderer, light.x, light.y, 20, 0xFF00FF00);
				filledCircleColor(renderer, culumn1.x, culumn1.y, culumn1.radius, 0xFF000000);
				circleColor(renderer, culumn1.x, culumn1.y, light.range, 0xFFFF00FF);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderPresent(renderer);
				break;
			}
			height_diff = culumn1.height - light.z;
			angle = atan2(light.x - culumn1.x, culumn1.y - light.y) + 1.57079f;
			column1_shadow_body.x1 = culumn1.x + sin(angle) * culumn1.radius;
			column1_shadow_body.y1 = culumn1.y - cos(angle) * culumn1.radius;
			column1_shadow_body.x2 = culumn1.x - sin(angle) * culumn1.radius;
			column1_shadow_body.y2 = culumn1.y + cos(angle) * culumn1.radius;
			column1_shadow_body.x3 = ((culumn1.height * light.x) - (column1_shadow_body.x2 * light.z)) / height_diff;
			column1_shadow_body.y3 = ((culumn1.height * light.y) - (column1_shadow_body.y2 * light.z)) / height_diff;
			column1_shadow_body.x4 = ((culumn1.height * light.x) - (column1_shadow_body.x1 * light.z)) / height_diff;
			column1_shadow_body.y4 = ((culumn1.height * light.y) - (column1_shadow_body.y1 * light.z)) / height_diff;

			shadow_far_edge_center.x = (column1_shadow_body.x4 + column1_shadow_body.x3) / 2;
			shadow_far_edge_center.y = (column1_shadow_body.y4 + column1_shadow_body.y3) / 2;
			shadow_tip_radius =
				sqrt(
					pow(shadow_far_edge_center.x - column1_shadow_body.x3, 2.f) +
					pow(shadow_far_edge_center.y - column1_shadow_body.y3, 2.f)
				);
			angle =
				atan2(column1_shadow_body.y4 - shadow_far_edge_center.y, column1_shadow_body.x4 - shadow_far_edge_center.x);

			// since filledPieColor requiers angles in degrees
			angle = rad_to_deg_approx(angle);

			SDL_SetRenderTarget(renderer, shadow_texture);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			light_rect.x = mouse_pos_x - light_rect.w / 2;
			light_rect.y = mouse_pos_y - light_rect.h / 2;
			SDL_RenderClear(renderer);
			// draw shadow
			filledPolygonColor(renderer, &column1_shadow_body.x1, &column1_shadow_body.y1, 4, 0xFFFFFFFF);
			// draw tip of shadow
			filledPieColor(renderer, shadow_far_edge_center.x, shadow_far_edge_center.y, shadow_tip_radius, angle - TIP_SHADOW_FIX_ANG, angle + 180 + TIP_SHADOW_FIX_ANG, 0xFFFFFFFF);
			// fill gap betwean tip and whadow
			thickLineColor(renderer, column1_shadow_body.x3, column1_shadow_body.y3, column1_shadow_body.x4, column1_shadow_body.y4, TIP_SHADOW_FIX_WIDTH, 0xFFFFFFFF);
			// make shadow softer
			SDL_RenderCopy(renderer, black_light_texture, NULL, &light_rect);

			SDL_SetRenderTarget(renderer, NULL);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, background_texture, NULL, NULL);
			SDL_RenderCopy(renderer, white_light_texture, NULL, &light_rect);
			SDL_RenderCopy(renderer, shadow_texture, NULL, NULL);
			filledCircleColor(renderer, culumn1.x, culumn1.y, culumn1.radius, 0xFFFF0000);
			//filledCircleColor(renderer, light.x, light.y, 20, 0xFF00FF00);
			//SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderPresent(renderer);
			break;
		case SDL_QUIT:
			goto free_up_memory_ad_return;
		}
free_up_memory_ad_return:*/
	kill_sdl(renderer, main_window);
	TTF_Quit();
	Mix_CloseAudio();

	#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
	#endif	//_DEBUG

	return EXIT_SUCCESS;
}
