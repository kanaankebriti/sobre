#include "sobre_engine.h"

// in: pointer to tiles array
// in: number of tiles in array
// in: target tile index
void rotate_tile_clockwise
(
	tiles* tile
)
{
	tile->orient = (tile->orient << 1) | (tile->orient >> 3);
	// example:
	// 0b00001010 ->
	// 0b00010101
	// but don't care about bit[7] ~ bit[4]!
}
