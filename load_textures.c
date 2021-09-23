#include "sobre_engine.h"

// in : renderer
// in : game number
// in out : number of textures
// return : array of textures
SDL_Texture** load_textures
(
	SDL_Renderer* renderer,
	unsigned char game_num,
	unsigned char *num_of_textures
)
{
	unsigned char i = 0;					// texture index
	char json_path[100];					// relative path to conf.json file
	char font_path[100];					// relative path to font file indicated in json texture files
	unsigned char json_path_len;			// length of jason_path characters
	unsigned char text_path_len;			// number characters of path to texture file
	char game_num_str[3];					// save game_num as string
	FILE* json_file = NULL;					// pointer for file operations
	char* json_content = NULL;				// json file in char array
	unsigned long json_file_len = 0;		// num of chars in json
	cJSON* cjson_conf_file_parsed = NULL;	// parsed conf.json file object
	cJSON* cjson_text_file_parsed = NULL;	// parsed x.json file object who contains text
	cJSON* cjson_texture_path;				// texture path property
	cJSON* cjson_texture_type;				// texture type property
	cJSON* cjson_texture_array = NULL;		// array of textures object
	cJSON* cjson_texture_item = NULL;		// object for each texture
	SDL_Surface* texture_surf = NULL;
	SDL_Texture** texture_bank;				// array of textures
	TTF_Font* ttf_font;						// TrueType font pointer for HUD texts
	SDL_Color text_colour;					// colour of HUD texts
	//SDL_Color text_bg_colour;				// colour of HUD texts' background

	// init number of textures
	*num_of_textures = 0;

	// game number to string
	_itoa(game_num, game_num_str, 10);

	// form path to conf.json file
	strcpy(json_path, ASSETS_PATH_HEADER);
	strcat(json_path, game_num_str);
	strcat(json_path, "/conf.json");

	// open json file
	json_file = fopen(json_path, "r");

	// remove "/conf.json" for later usage
	json_path_len = (unsigned char)strlen(json_path);
	json_path[json_path_len - 10] = '\0';

	// read conf.json
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
		cjson_conf_file_parsed = cJSON_Parse(json_content);
		if (cjson_conf_file_parsed == NULL)
			return NULL;
	}
	else
		return NULL;

	// read 'textures' list
	cjson_texture_array = cJSON_GetObjectItem(cjson_conf_file_parsed, "textures");
	*num_of_textures = (unsigned char)cJSON_GetArraySize(cjson_texture_array);

	// allocate memory respect to *num_of_textures
	texture_bank = malloc(*num_of_textures * sizeof(SDL_Texture*));				// create texture array
	if (!texture_bank)														// check for null pointer
		return NULL;

	cjson_texture_item = cjson_texture_array ? cjson_texture_array->child : 0;

	// load textures
	while (cjson_texture_item)
	{
		cjson_texture_path = cJSON_GetObjectItem(cjson_texture_item, "path");
		cjson_texture_type = cJSON_GetObjectItem(cjson_texture_item, "type");

		// path of textures are relative to conf.json
		strcat(json_path, cjson_texture_path->valuestring);

		// load textures based on type
		if (!strcmp(cjson_texture_type->valuestring, "bmp"))					// create bmp surface
			texture_surf = SDL_LoadBMP(json_path);
		else if (!strcmp(cjson_texture_type->valuestring, "png"))				// create png surface
			texture_surf = IMG_Load(json_path);
		else if (!strcmp(cjson_texture_type->valuestring, "svg"))				// create svg surface
			texture_surf = IMG_Load(json_path);
		else if (!strcmp(cjson_texture_type->valuestring, "jpg"))				// create jpg surface
			texture_surf = IMG_Load(json_path);
		else if (!strcmp(cjson_texture_type->valuestring, "jpeg"))				// create jpeg surface
			texture_surf = IMG_Load(json_path);
		else if (!strcmp(cjson_texture_type->valuestring, "text"))				// create surface containg text
		{
			// open json file
			// for example 27.json
			json_file = fopen(json_path, "r");

			// free up previous contents of json_content char array
			free(json_content);

			// read x.json
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
				cjson_text_file_parsed = cJSON_Parse(json_content);
				if (cjson_conf_file_parsed == NULL)
					return NULL;
			}
			else
				return NULL;

			// retrive and assign text colour from json
			text_colour.r = (Uint8)cJSON_GetObjectItem(cjson_text_file_parsed, "r")->valueint;
			text_colour.g = (Uint8)cJSON_GetObjectItem(cjson_text_file_parsed, "g")->valueint;
			text_colour.b = (Uint8)cJSON_GetObjectItem(cjson_text_file_parsed, "b")->valueint;
			text_colour.a = (Uint8)cJSON_GetObjectItem(cjson_text_file_parsed, "a")->valueint;

			// form path to font file
			strcpy(font_path, ASSETS_FONTS_PATH);
			strcat(font_path, cJSON_GetObjectItem(cjson_text_file_parsed, "font")->valuestring);

			// creating font based on data from json
			ttf_font =
				TTF_OpenFont
				(
					font_path,
					cJSON_GetObjectItem(cjson_text_file_parsed, "size")->valueint
				);

			// create a texture who contains the text using preset settings
			//text_bg_colour.a = 255;// text's background color is always just transparent!
			texture_surf =
				TTF_RenderText_Blended
				(
					ttf_font,
					cJSON_GetObjectItem(cjson_text_file_parsed, "text")->valuestring,
					text_colour
				);

			// TODO: oh my god! how can i solve this elusive problem :(
			// how can i integrate font size from hud.conf with those from xxx.json textures :(
			// for now, i check for it using code below
			/*int w, h;
			char ws[15], hs[15], res[50];
			TTF_SizeText(ttf_font, "Explode", &w, &h);
			_itoa(w, ws, 10);
			_itoa(h, hs, 10);
			strcpy(res, ws);
			strcat(res, "\t");
			strcat(res, hs);
			strcat(res, "\n");
			OutputDebugStringA(res);*/


			// destroy the font or it will lead to memory leaks
			TTF_CloseFont(ttf_font);

			// reset font_path
			font_path[0] = '\0';

			// free up memory
			cJSON_Delete(cjson_text_file_parsed);
		}
		else
			return NULL;														// failed to load texture

		texture_bank[i] = SDL_CreateTextureFromSurface(renderer, texture_surf);	// create texture

		SDL_FreeSurface(texture_surf);											// free up surface or it will lead to memory leak

		json_path_len = (unsigned char)strlen(json_path);						// trim filename from json_path
		text_path_len = (unsigned char)strlen(cjson_texture_path->valuestring);
		json_path[json_path_len - text_path_len] = '\0';

		cjson_texture_item = cjson_texture_item->next;
		i++;
	}

	// free up memory
	cJSON_Delete(cjson_conf_file_parsed);
	free(json_content);

	return texture_bank;
}
