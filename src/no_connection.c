#include "sobre_engine.h"

bool is_up_connected
(
	tiles tile1,
	tiles tile2
)
{
	// tile1: XXX1 or XXX0 (mask)-> 0001 or 0000
	// tile2: X1XX or X0XX (mask)-> 0100 or 0000
	if ((tile1.orient & UP_ORIENT) && (tile2.orient & DOWN_ORIENT))
		return true;
	return false;
}

bool is_right_connected
(
	tiles tile1,
	tiles tile2
)
{
	// tile1: XX1X or XX0X (mask)-> 0010 or 0000
	// tile2: 1XXX or 0XXX (mask)-> 1000 or 0000
	if ((tile1.orient & RIGHT_ORIENT) && (tile2.orient & LEFT_ORIENT))
		return true;
	return false;
}

bool is_down_connected
(
	tiles tile1,
	tiles tile2
)
{
	// tile1: X1XX or X0XX (mask)-> 0100 or 0000
	// tile2: XXX1 or XXX0 (mask)-> 0001 or 0000
	if ((tile1.orient & DOWN_ORIENT) && (tile2.orient & UP_ORIENT))
		return true;
	return false;
}

bool is_left_connected
(
	tiles tile1,
	tiles tile2
)
{
	// tile1: 1XXX or 0XXX (mask)-> 1000 or 0000
	// tile2: XX1X or XX0X (mask)-> 0010 or 0000
	if ((tile1.orient & LEFT_ORIENT) && (tile2.orient & RIGHT_ORIENT))
		return true;
	return false;
}

// return true when there exist no connection between tiles
bool no_connection
(
	tiles* tile,
	unsigned char width,
	unsigned char height
)
{
	unsigned short int i, j;

	// first row
	for (i = 0; i < width; i++)
	{
		if (!i)					// upper left corner
		{
			if (height == 1)	// one row only condition
			{
				if (is_right_connected(tile[0], tile[1]))
					return false;
			}
			else				// multi row
			{
				if (is_right_connected(tile[0], tile[1]) ||
					is_down_connected(tile[0], tile[width]))
					return false;
			}
		}
		else if (i == width - 1)// upper right corner
		{
			if (height == 1)	// one row only condition
			{
				if (is_left_connected(tile[width - 1], tile[width - 2]))
					return false;
			}
			else				// multi row
			{
				if (is_left_connected(tile[width - 1], tile[width - 2]) ||
					is_down_connected(tile[width - 1], tile[i + width]))
					return false;
			}
		}
		else					// other tiles in first row
		{
			if (height == 1)	// one row only condition
			{
				if (is_right_connected(tile[i], tile[i + 1]) ||
					is_left_connected(tile[i], tile[i - 1]))
					return false;
			}
			else				// multi row
			{
				if (is_right_connected(tile[i], tile[i + 1]) ||
					is_left_connected(tile[i], tile[i - 1]) ||
					is_down_connected(tile[i], tile[i + width]))
					return false;
			}
		}
	}

	// between first row and last row
	// automatically cancels out in one row condition
	for (i = width; i <= width * (height - 2); i += width)
	{
		for (j = 0; j < width; j++)
		{
			if (!j)						// most left
			{
				if (is_right_connected(tile[i], tile[i + 1]) ||
					is_down_connected(tile[i], tile[i + width]))
					return false;
			}
			else if (j == width - 1)	// most right
			{
				if (is_down_connected(tile[i + j], tile[i + j + width]) ||
					is_left_connected(tile[i + j], tile[i + j - 1]))
					return false;
			}
			else						// almost all tiles fall into this category
			{
				if (is_right_connected(tile[i + j], tile[i + j + 1]) ||
					is_down_connected(tile[i + j], tile[i + j + width]) ||
					is_left_connected(tile[i + j], tile[i + j - 1]))
					return false;
			}
		}
	}

	// last row
	// automatically cancels out in one row condition
	// note that last tile needs no check
	for (i = width * (height - 1); i < width * height - 1; i++)
	{
		if (i == width * (height - 1))	// lower left corner
		{
			if (is_right_connected(tile[i], tile[i + 1]))
				return false;
		}
		else if (i == width * height - 1)// lower right corner
		{
			if (is_left_connected(tile[i], tile[i - 1]))
				return false;
		}
		else
		{
			if (is_right_connected(tile[i], tile[i + 1]) ||
				is_left_connected(tile[i], tile[i - 1]))
				return false;
		}
	}

	return true;
}
