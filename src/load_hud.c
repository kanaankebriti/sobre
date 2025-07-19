#include "sobre_engine.h"

// in: game number
// in out: number of all tiles
// return: pointer to tiles array
tiles* load_hud
(
	unsigned char game_num,
	unsigned char* hud_size
)
{
	tiles* tile = NULL;						// pointer to array of tiles
	unsigned char i = 0;					// index
	unsigned char j = 0;					// animation textures index. supports 255 animations indices
	char j_str[4];							// animation textures index null terminated string. txt_0 to txt_255
	char game_num_str[3];					// save game_num as string
	char json_path[100] = { 0 };			// relative path to file
	char* json_content = 0;					// json file in char array
	char txt_anim_index[7] = { "txt_\0" };	// texture animation index properties from json
	unsigned long json_file_len = 0;		// num of chars in json
	FILE* json_file = NULL;					// pointer for file operations
	cJSON* cjson_parsed = NULL;				// parsed json file object
	cJSON* cjson_tile_array = NULL;			// array of tiles object
	cJSON* cjson_tile_item = NULL;			// object for each tile
	cJSON* cjson_text_array = NULL;			// array of text object
	cJSON* cjson_text_item = NULL;			// object for each text
	cJSON* cjson_digit_array = NULL;			// array of digit tile object
	cJSON* cjson_digit_item = NULL;			// object for each digit tile
	cJSON* cjson_tile_x;					// tile x property
	cJSON* cjson_tile_y;					// tile y property
	cJSON* cjson_tile_w;					// tile width property
	cJSON* cjson_tile_h;					// tile height property
	cJSON* cjson_tile_ang;					// tile angle property
	cJSON* cjson_tile_txt_anim;				// tile texture animation index property (might have no animation!)
	cJSON* cjson_tile_txt_anim_size;		// number of tile texture animation index property

	// game number to string
	#ifdef _WIN32
		// windows internals
		_itoa(game_num, game_num_str, 10);
	#else
		// standard c library
		sprintf(game_num_str, "%hhu", game_num);
	#endif

	// form path to hud.json file
	strcat(json_path, ASSETS_PATH_HEADER);
	strcat(json_path, game_num_str);
	strcat(json_path, "/hud.json");

	// open json file
	json_file = fopen(json_path, "r");

	// read json
	if (json_file)
	{
		fseek(json_file, 0, SEEK_END);
		json_file_len = ftell(json_file);
		fseek(json_file, 0, SEEK_SET);
		json_content = malloc(json_file_len + 1);								// +1 for '\0'
		if (json_content)
			fread(json_content, 1, json_file_len, json_file);
		json_content[json_file_len] = '\0';										// null-termination
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

	// read lists od hud.json
	cjson_tile_array = cJSON_GetObjectItem(cjson_parsed, "tiles");
	cjson_text_array = cJSON_GetObjectItem(cjson_parsed, "texts");
	cjson_digit_array = cJSON_GetObjectItem(cjson_parsed, "digits");
	*hud_size =		(unsigned char)cJSON_GetArraySize(cjson_tile_array)
				+	(unsigned char)cJSON_GetArraySize(cjson_text_array)
				+	(unsigned char)cJSON_GetArraySize(cjson_digit_array);
	tile = malloc((*hud_size) * sizeof(tiles));							// create tile array
	if (!tile)															// check for null pointer
		return NULL;

	// populate tiles
	cjson_tile_item = cjson_tile_array ? cjson_tile_array->child : 0;
	while (cjson_tile_item)
	{
		// retrive tile properties from JSON
		cjson_tile_x = cJSON_GetObjectItem(cjson_tile_item, "x");
		cjson_tile_y = cJSON_GetObjectItem(cjson_tile_item, "y");
		cjson_tile_w = cJSON_GetObjectItem(cjson_tile_item, "w");
		cjson_tile_h = cJSON_GetObjectItem(cjson_tile_item, "h");
		cjson_tile_ang = cJSON_GetObjectItem(cjson_tile_item, "ang");
		cjson_tile_txt_anim_size = cJSON_GetObjectItem(cjson_tile_item, "txt_cnt");

		// assign retrived info to struct members
		tile[i].rect.x = cjson_tile_x->valueint;						// relative x position
		tile[i].rect.y = cjson_tile_y->valueint;						// relative y position
		tile[i].rect.w = cjson_tile_w->valueint;						// width
		tile[i].rect.h = cjson_tile_h->valueint;						// height
		tile[i].angle = (unsigned short int)cjson_tile_ang->valueint;	// orientation

		// retrive and malloc animation textures
		tile[i].texture_index_anim_size = (unsigned char)cjson_tile_txt_anim_size->valueint;
		tile[i].texture_index_anim = malloc(sizeof(unsigned char) * cjson_tile_txt_anim_size->valueint);

		// load animation textures indices
		for (j = 0; j < cjson_tile_txt_anim_size->valueint; j++)
		{
			#ifdef _WIN32	// windows internals
				_itoa(j, j_str, 10);		// form txt_anim_index name
			#else			// standard c library
				sprintf(j_str, "%hhu", j);
			#endif
			strcat(txt_anim_index, j_str);
			// retrieve txt_anim_index from json
			cjson_tile_txt_anim = cJSON_GetObjectItem(cjson_tile_item, txt_anim_index);
			// copy txt_anim_index to tile struct
			tile[i].texture_index_anim[j] = (unsigned char)cjson_tile_txt_anim->valueint;
			// reset txt_anim_index
			strcpy(txt_anim_index, "txt_\0");
		}

		tile[i].texture_index_cur = tile[i].texture_index_anim[0];		// set current texture index

		cjson_tile_item = cjson_tile_item->next;
		i++;
	}

	// populate texts
	cjson_text_item = cjson_text_array ? cjson_text_array->child : 0;
	while (cjson_text_item)
	{
		// retrive tile properties from JSON
		cjson_tile_x = cJSON_GetObjectItem(cjson_text_item, "x");
		cjson_tile_y = cJSON_GetObjectItem(cjson_text_item, "y");
		cjson_tile_w = cJSON_GetObjectItem(cjson_text_item, "w");
		cjson_tile_h = cJSON_GetObjectItem(cjson_text_item, "h");
		cjson_tile_ang = cJSON_GetObjectItem(cjson_text_item, "ang");
		cjson_tile_txt_anim = cJSON_GetObjectItem(cjson_text_item, "txt");

		tile[i].rect.x = cjson_tile_x->valueint;									// relative x position
		tile[i].rect.y = cjson_tile_y->valueint;									// relative y position
		tile[i].rect.w = cjson_tile_w->valueint;									// width
		tile[i].rect.h = cjson_tile_h->valueint;									// height
		tile[i].angle = (unsigned short int)cjson_tile_ang->valueint;				// orientation
		tile[i].texture_index_cur = (unsigned char)cjson_tile_txt_anim->valueint;	// set current texture index

		// HUD text has no animation! only one texture
		tile[i].texture_index_anim_size = 0;
		tile[i].texture_index_anim = malloc(sizeof(unsigned char));
		tile[i].texture_index_anim[0] = (unsigned char)cjson_tile_txt_anim->valueint;

		cjson_text_item = cjson_text_item->next;
		i++;
	}

	// populate digit tiles
	cjson_digit_item = cjson_digit_array ? cjson_digit_array->child : 0;
	while (cjson_digit_item)
	{
		// retrive tile properties from JSON
		cjson_tile_x = cJSON_GetObjectItem(cjson_digit_item, "x");
		cjson_tile_y = cJSON_GetObjectItem(cjson_digit_item, "y");
		cjson_tile_w = cJSON_GetObjectItem(cjson_digit_item, "w");
		cjson_tile_h = cJSON_GetObjectItem(cjson_digit_item, "h");
		cjson_tile_ang = cJSON_GetObjectItem(cjson_digit_item, "ang");

		tile[i].rect.x = cjson_tile_x->valueint;						// relative x position
		tile[i].rect.y = cjson_tile_y->valueint;						// relative y position
		tile[i].rect.w = cjson_tile_w->valueint;						// width
		tile[i].rect.h = cjson_tile_h->valueint;						// height
		tile[i].angle = (unsigned short int)cjson_tile_ang->valueint;	// orientation
		tile[i].texture_index_cur = 0;									// all digits start with 0

		// HUD digits has no animation! only one intial texture which is number zero
		tile[i].texture_index_anim_size = 0;
		tile[i].texture_index_anim = NULL;

		cjson_digit_item = cjson_digit_item->next;
		i++;
	}

	// free up memory
	cJSON_Delete(cjson_parsed);
	free(json_content);

	return tile;
}