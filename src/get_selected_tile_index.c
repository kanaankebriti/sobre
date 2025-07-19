#include "sobre_engine.h"

// search for selected tile index
// on failure returns number_of_tiles
unsigned char get_selected_tile_index
(
	tiles* tile,
	unsigned char number_of_tiles,
	int mouse_click_x,
	int mouse_click_y
)
{
	unsigned char i;

	// 'i' will be the selected tile index
	for (i = 0; i < number_of_tiles; i++)
		if (mouse_click_x > tile[i].rect.x &&					// greater than left side
			mouse_click_x < tile[i].rect.x + tile[i].rect.w &&	// less than right side
			mouse_click_y > tile[i].rect.y &&					// greater than top side
			mouse_click_y < tile[i].rect.y + tile[i].rect.h)	// less than bottom side
			return i;
	return number_of_tiles;
}
