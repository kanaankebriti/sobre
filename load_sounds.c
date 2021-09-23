#include "sobre_engine.h"

// in: game number
// in out: number of sounds including last NULL element
// return: NULL terminated array of sounds
Mix_Chunk** load_sounds
(
	unsigned char game_num,
	unsigned char *num_of_sounds
)
{
	Mix_Chunk** sounds_bank;				// array of sounds
	unsigned char i = 0;					// sound index
	unsigned char json_path_len;			// length of jason_path characters
	unsigned char sound_path_len;			// number characters of path to sound file
	char game_num_str[3];					// save game_num as string
	char json_path[100];					// relative path to conf.json file
	char* json_content = NULL;				// json file in char array
	unsigned long json_file_len = 0;		// num of chars in json
	FILE* json_file = NULL;					// pointer for file operations
	cJSON* cjson_conf_file_parsed = NULL;	// parsed conf.json file object
	cJSON* cjson_sounds_array = NULL;		// array of sounds object
	cJSON* cjson_sound_item = NULL;			// object for each sounds
	cJSON* cjson_sound_path;				// sound path property
	cJSON* cjson_sound_type;				// sound type property

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

	// read 'sounds' list
	cjson_sounds_array = cJSON_GetObjectItem(cjson_conf_file_parsed, "sounds");
	*num_of_sounds = (unsigned char)cJSON_GetArraySize(cjson_sounds_array);

	// allocate memory respect to num_of_texts
	sounds_bank = malloc(++*num_of_sounds * sizeof(Mix_Chunk*));					// create texture array
	if (!sounds_bank)															// check for null pointer
		return NULL;

	cjson_sound_item = cjson_sounds_array ? cjson_sounds_array->child : 0;

	// load textures
	while (cjson_sound_item)
	{
		cjson_sound_path = cJSON_GetObjectItem(cjson_sound_item, "path");
		cjson_sound_type = cJSON_GetObjectItem(cjson_sound_item, "type");

		// path of sounds are relative to conf.json
		strcat(json_path, cjson_sound_path->valuestring);

		if (!strcmp(cjson_sound_type->valuestring, "ogg"))
			sounds_bank[i] = Mix_LoadWAV(json_path);
		else if (!strcmp(cjson_sound_type->valuestring, "mp3"))
			sounds_bank[i] = Mix_LoadWAV(json_path);
		else if (!strcmp(cjson_sound_type->valuestring, "wav"))
			sounds_bank[i] = Mix_LoadWAV(json_path);
		else
			return NULL;														// audio type not supported

		json_path_len = (unsigned char)strlen(json_path);						// trim filename from json_path
		sound_path_len = (unsigned char)strlen(cjson_sound_path->valuestring);
		json_path[json_path_len - sound_path_len] = '\0';

		cjson_sound_item = cjson_sound_item->next;
		i++;
	}

	// NULL termination
	sounds_bank[i] = NULL;

	// free up memory
	cJSON_Delete(cjson_conf_file_parsed);
	free(json_content);

	return sounds_bank;
}
