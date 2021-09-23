#include "sobre_engine.h"

// in: game number
// in: level number
// in out: number of columns
// in out: number of rows
// return: pointer to tiles array
tiles* load_level
(
	unsigned char game_num,
	unsigned char lvl_num,
	unsigned char* lvl_width,
	unsigned char* lvl_height
)
{
	tiles* tile = NULL;						// pointer to array of tiles
	unsigned char lvl_size;					// number of all tiles
	unsigned char i = 0;					// index
	unsigned char shift_to_right_times = 0;	// counter
	char lvl_num_str[3];					// save lvl_num as string
	char game_num_str[3];					// save game_num as string
	char json_path[100];					// relative path to file
	char* json_content = 0;					// json file in char array
	unsigned long json_file_len = 0;		// num of chars in json
	FILE* json_file = NULL;					// pointer for file operations
	cJSON* cjson_parsed = NULL;				// parsed json file object
	cJSON* cjson_tile_array = NULL;			// array of tiles object
	cJSON* cjson_tile_item = NULL;			// object for each tile
	cJSON* cjson_lvl_w;						// level width
	cJSON* cjson_lvl_h;						// level height
	cJSON* cjson_tile_x;					// tile x property
	cJSON* cjson_tile_y;					// tile y property
	cJSON* cjson_tile_w;					// tile width property
	cJSON* cjson_tile_h;					// tile height property
	cJSON* cjson_tile_ang;					// tile angle property
	cJSON* cjson_tile_txt;					// tile texture index property
	cJSON* cjson_tile_type;					// tile type property

	// game number to string
	_itoa(game_num, game_num_str, 10);
	// level number to string
	_itoa(lvl_num, lvl_num_str, 10);

	// form path to lvl[x].json file
	strcpy(json_path, ASSETS_PATH_HEADER);
	strcat(json_path, game_num_str);
	strcat(json_path, "/lvl");
	strcat(json_path, lvl_num_str);
	strcat(json_path, ".json");

	// open json file
	json_file = fopen(json_path, "r");

	// read json
	if (json_file)
	{
		fseek(json_file, 0, SEEK_END);
		json_file_len = ftell(json_file);
		fseek(json_file, 0, SEEK_SET);
		json_content = malloc(json_file_len * sizeof(char));
		if (json_content)
		{
			fread(json_content, 1, json_file_len, json_file);
		}
		fclose(json_file);
	}

	// parse json
	if (json_content)
	{
		cjson_parsed = cJSON_Parse(json_content);
		if (cjson_parsed == NULL)
			return NULL;
	}
	else
		return NULL;

	// extract json data
	cjson_lvl_w = cJSON_GetObjectItem(cjson_parsed, "lvl_w");
	*lvl_width = (unsigned char)cjson_lvl_w->valueint;
	cjson_lvl_h = cJSON_GetObjectItem(cjson_parsed, "lvl_h");
	*lvl_height = (unsigned char)cjson_lvl_h->valueint;

	lvl_size = (*lvl_width) * (*lvl_height);							// size of level = width * height
	if (!lvl_size)														// check for zero lvl_size
		return NULL;
	tile = malloc(lvl_size * sizeof(tiles));							// create tile array. start from 0 therefore -- is needed.
	if (!tile)															// check for null pointer
		return NULL;
	cjson_tile_array = cJSON_GetObjectItem(cjson_parsed, "tiles");

	cjson_tile_item = cjson_tile_array ? cjson_tile_array->child : 0;

	// populate tiles
	while (cjson_tile_item)
	{
		cjson_tile_x = cJSON_GetObjectItem(cjson_tile_item, "x");
		cjson_tile_y = cJSON_GetObjectItem(cjson_tile_item, "y");
		cjson_tile_w = cJSON_GetObjectItem(cjson_tile_item, "w");
		cjson_tile_h = cJSON_GetObjectItem(cjson_tile_item, "h");
		cjson_tile_ang = cJSON_GetObjectItem(cjson_tile_item, "ang");
		cjson_tile_txt = cJSON_GetObjectItem(cjson_tile_item, "txt");
		cjson_tile_type = cJSON_GetObjectItem(cjson_tile_item, "type");

		// if type attribute has been not specified
		// do not add any new tile
		if (cjson_tile_type)
		{
			tile[i].rect.x = cjson_tile_x->valueint;							// relative x position
			tile[i].rect.y = cjson_tile_y->valueint;							// relative y position
			tile[i].rect.w = cjson_tile_w->valueint;							// width
			tile[i].rect.h = cjson_tile_h->valueint;							// height
			tile[i].angle = (unsigned short int)cjson_tile_ang->valueint;		// orientation
			tile[i].texture_index_cur = (unsigned char)cjson_tile_txt->valueint;// current texture index
			tile[i].texture_index_anim = NULL;									// currently level tiles contain no animation!
			tile[i].texture_index_anim_size = 0;								// currently level tiles contain no animation!

			// assign tile orientation information
			// could be up, right, down, left
			// or a combination of them
			switch (cjson_tile_type->valueint)
			{
			case 0:
				tile[i].orient = NO_ORIENT;
				break;
			case 1:
				tile[i].orient = UP_ORIENT; // orient = 1
				break;
			case 2:
				tile[i].orient = UP_ORIENT | DOWN_ORIENT; // orient = 5
				break;
			case 3:
				tile[i].orient = UP_ORIENT | RIGHT_ORIENT; // orient = 3
				break;
			case 4:
				tile[i].orient = UP_ORIENT | RIGHT_ORIENT | LEFT_ORIENT; // orient = 11
				break;
			case 5:
				tile[i].orient = UP_ORIENT | RIGHT_ORIENT | DOWN_ORIENT | LEFT_ORIENT; // orient = 15
				break;
			}

			// take into account the original orientation
			// using shift to right
			switch (cjson_tile_ang->valueint)
			{
			case 90:
				shift_to_right_times++;
				break;
			case 180:
				shift_to_right_times = 2;
				break;
			case 270:
				shift_to_right_times = 3;
				break;
			}
			while (shift_to_right_times != 0)
			{
				rotate_tile_clockwise(&tile[i]);
				shift_to_right_times--;
			}
		}
		else
		{
			// dummy tile
			tile[i].orient = NO_ORIENT;
			tile[i].rect.w = 0;			// zero width indicates no render is needed
		}

		cjson_tile_item = cjson_tile_item->next;
		i++;
	}

	// free up memory
	cJSON_Delete(cjson_parsed);
	free(json_content);

	return tile;
}
