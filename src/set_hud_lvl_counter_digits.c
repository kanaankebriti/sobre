#include "sobre_engine.h"

void set_hud_lvl_counter_digits
(
	tiles* hud_tiles,
	unsigned short int number
)
{
	unsigned char i = HUD_FIRST_DIGIT_ID;
	do
	{
		hud_tiles[i].texture_index_cur = number % 10;
		i++;
	} while ((number /= 10) > 0 /* && i < HUD_LAST_DIGIT_ID */);
}