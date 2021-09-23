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
		// greater than left side
		if (mouse_click_x > tile[i].rect.x)
			// less than right side
			if(mouse_click_x < tile[i].rect.x + tile[i].rect.w)
				// greater than top side
				if(mouse_click_y > tile[i].rect.y)
					// less than bottom side
					if(mouse_click_y < tile[i].rect.y + tile[i].rect.h)
						return i;

	return number_of_tiles;
}
