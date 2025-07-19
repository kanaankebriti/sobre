#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>

#ifdef _WIN32
	#ifdef _DEBUG
		#define _CRTDBG_MAP_ALLOC
		#include <crtdbg.h>
		#ifndef DBG_NEW
			#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
			#define new DBG_NEW
		#endif
	#endif	// _DEBUG

	#include "resource.h"
	#include <windows.h>
#endif	// _WIN32

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_thread.h>
#include <cjson/cJSON.h>

#define WIN_TITLE_STR "sobre"
#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
//#define ROTATION_FRAMERATE 33		// (1000/30) = 33.3 or 30 Frame Per Second
#define ROTATION_ANIMATION_INCEREMENT 15
//#define LR_WAVE_FRAMERATE 10		// (1000/10) = 100 Frame Per Second
#define LR_WAVE_START_WIDTH 100
//#define GC_WAVE_FRAMERATE 5		// (1000/5) = 200 Frame Per Second
#define GC_WAVE_END_PERC 1.8		// 80% more increment after wave touches edge of screen
#define GC_WAVE_START_PERC 1.2		// 20% more when shirinking from full circle toward zero radius at center
#define GC_WAVE_FADE_IN_STEP 60
#define GC_WAVE_FADE_OUT_STEP 60
#define SWEEPER_WAVE_ANIMATION_INCEREMENT 30
#define HOVER_DELAY 100				// helps to have waitevent and hover together without flickering due to high refresh rate
#define FADE_TO_LEFT_STEP 15
#define COLOR_MAX_ILL 200			// maximum colour illumination
#define COLOR_MIN_ILL 100			// minimum colour illumination
//#define FADE_TO_LEFT_FRAMERATE 17	// (1000/60) = 16.7 or 30 Frame Per Second
#define HUD_FADE_IN_OUT_STEP 10
#define MAX_TRANSPARENCY 255
#define MIN_TRANSPARENCY_LVL_TILES 100
#define MIN_TRANSPARENCY_HUD_TILES 10
#define HUD_TILE_FADE_IN_OUT_DELAY 40
#define NOTE_MIN_WAIT_MS 3000
#define NOTE_MAX_WAIT_MS 6000
#define ASSETS_PATH_HEADER "./res/g"
#define ASSETS_FONTS_PATH "./res/fonts/"
#define NO_ORIENT		0b00000000
#define UP_ORIENT		0b00000001
#define RIGHT_ORIENT	0b00000010
#define DOWN_ORIENT		0b00000100
#define LEFT_ORIENT		0b00001000
#define CMD_NULL 0
#define CMD_RUN_MAIN_MENU 1
#define CMD_RUN_GAME_1 2
#define CMD_RESUME_GAME 251
#define CMD_RUN_NEXT_LVL 252
#define CMD_CLOSE_HUD 253
#define CMD_END_GAME 254
#define CMD_RUN_FAIL 255

#define SINGLE_BMP_WHITE_DOT_TEXTURE_ID 10
#define CIRCLE_TEXTURE_ID 11
#define FIRST_COMMON_TILE_TEXTURE_ID 12
#define LAST_COMMON_TILE_TEXTURE_ID 19
#define FIRST_LVL_TILE_TEXTURE_ID 12
#define LAST_LVL_TILE_TEXTURE_ID 16
#define HUD_MAIN_BTN_ID 0
#define HUD_MAIN_MENU_BTN_ID 1
#define HUD_FIRST_ANIMATING_BTN_ID 1
#define HUD_LAST_ANIMATING_BTN_ID 2
#define HUD_FIRST_DIGIT_ID 3
#define HUD_LAST_DIGIT_ID 6

#define FIRST_SOUND_ID 1

// inefficient but
// this struct works for
// level tiles
// HUD tiles
typedef struct tiles
{
	// orient : 1 = up
	// orient : 2 = right
	// orient : 4 = down
	// orient : 8 = left
	// mix orients using OR e.g. UP and DOWN = 5
	unsigned char orient;						// used to check for connection
	unsigned char* texture_index_anim;			// array of texture indices on animation
	unsigned char texture_index_anim_size;		// number of texture indices on animation
	unsigned char texture_index_cur;			// current texture index
	unsigned short int angle;					// angle respect to center
	unsigned char id;							// assign id to each tile, so it could be addressed later by scripts.
	char script_path[32];						// this script will be executed onClick.
	SDL_Rect rect;								// x, y, width, height
} tiles;

unsigned char get_selected_tile_index(tiles* tile, unsigned char number_of_tiles, int mouse_click_x, int mouse_click_y);
bool init_sdl(SDL_Renderer** renderer, SDL_Window** window);
void kill_sdl(SDL_Renderer* renderer, SDL_Window* main_window);
tiles* load_hud(unsigned char game_num, unsigned char* hud_size);
tiles* load_level(unsigned char game_num, unsigned char lvl_num, unsigned char* lvl_width, unsigned char* lvl_height);
Mix_Chunk** load_sounds(unsigned char game_num, unsigned char* num_of_sounds);
SDL_Texture** load_textures(SDL_Renderer* renderer, unsigned char game_num, unsigned char* num_of_textures);
bool no_connection(tiles* tile, unsigned char width, unsigned char height);
void render_clear(SDL_Renderer* renderer, unsigned char r, unsigned char b, unsigned char g);
void render_clockwise_rotation(SDL_Renderer* renderer, tiles* tile, unsigned char number_of_tiles, unsigned char target, SDL_Texture* texture_bank[], tiles* hud_tiles);
void render_counterclockwise_rotation(SDL_Renderer* renderer, tiles* tile, unsigned char number_of_tiles, unsigned char target, SDL_Texture* texture_bank[], tiles* hud_tiles);
void render_fade_in_gc_wave(SDL_Renderer* renderer, tiles* tile, unsigned char number_of_tiles, SDL_Texture* texture_bank[], tiles* hud_tiles);
void render_fade_in_hud_from_bottom(SDL_Renderer* renderer, tiles* tile, unsigned char num_of_tiles, SDL_Texture* texture_bank[], tiles* hud_tiles);
void render_fade_out_hud_from_bottom(SDL_Renderer* renderer, tiles* tile, unsigned char num_of_tiles, SDL_Texture* texture_bank[], tiles* hud_tiles);
void render_fade_out_to_left(SDL_Renderer* renderer, tiles* tile, unsigned char number_of_tiles, SDL_Texture* texture_bank[]);
void render_still_layer(SDL_Renderer* renderer, tiles* tile, unsigned char num_of_tiles, SDL_Texture* texture_bank[]);
void render_still_layer_with_hud(SDL_Renderer* renderer, tiles* tile, unsigned char num_of_tiles, SDL_Texture* texture_bank[], tiles* hud_tiles, unsigned char hud_size);
void render_hover_on_hud_fade_in(SDL_Renderer* renderer, SDL_Texture* texture_bank[], tiles* hud_tiles, unsigned char hud_size, unsigned char target_hud_index);
void render_hover_on_hud_fade_in_with_lvl_tiles(SDL_Renderer* renderer, SDL_Texture* texture_bank[], tiles* hud_tiles, unsigned char hud_size, unsigned char target_hud_index, tiles* lvl_tiles, unsigned char num_of_tiles);
void render_hover_on_hud_fade_out(SDL_Renderer* renderer, SDL_Texture* texture_bank[], tiles* hud_tiles, unsigned char hud_size);
void render_hover_on_hud_fade_out_with_lvl_tiles(SDL_Renderer* renderer, SDL_Texture* texture_bank[], tiles* hud_tiles, unsigned char hud_size, tiles* lvl_tiles, unsigned char num_of_tiles);
void render_lr_wave(SDL_Renderer* renderer, tiles* tile, unsigned char number_of_tiles, SDL_Texture* texture_bank[], tiles* hud_tiles);
void render_fore_gc_wave_from_tile(SDL_Renderer* renderer, tiles* tile, SDL_Texture* texture_bank[], unsigned char center_tile_index);
void render_fore_gc_wave_from_xy(SDL_Renderer* renderer, SDL_Texture* texture_bank[], int x, int y);
void render_back_gc_wave_from_tile_with_hud(SDL_Renderer* renderer, tiles* tile, unsigned char number_of_tiles, SDL_Texture* texture_bank[], unsigned char center_tile_index, tiles* hud_tiles);
void rotate_tile_clockwise(tiles* tile);
void rotate_tile_counterclockwise(tiles* tile);
unsigned char run_game_1(SDL_Renderer* renderer);
unsigned char run_main_menu(SDL_Renderer* renderer);
void set_hud_lvl_counter_digits(tiles* hud_tiles, unsigned short int number);
