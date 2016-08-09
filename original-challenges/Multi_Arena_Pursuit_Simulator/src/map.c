/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "map.h"
#include "malloc.h"
#include <libcgc.h>

/**
 * Create a new square map with a bottom left and top right boundary
 * @param  bottomLeft The bottom most left most coordinate of the map
 * @param  topRight   The top most right most coordinate of the map
 * @return            A pointer of the newly created map
 */
cgc_Map* cgc_newMap(cgc_Coordinate bottomLeft, cgc_Coordinate topRight) {
	cgc_Map* map;
	int minX, maxX;
	int minY, maxY;
	int xWidth, yWidth;

	minX = bottomLeft.x;
	maxX = topRight.x;
	minY = bottomLeft.y;
	maxY = topRight.y;

	if(!(map = cgc_malloc(sizeof(cgc_Map))))
		_terminate(ALLOCATE_ERROR);

	map->bottomLeft = bottomLeft;
	map->topRight = topRight;

	xWidth = maxX - minX + 1;
	yWidth = maxY - minY + 1;

	if(!(map->data = cgc_malloc(xWidth * yWidth * sizeof(unsigned int)))) 
		_terminate(ALLOCATE_ERROR);

	for(int c=0; c<=xWidth; c++) {
		for(int r=0; r<=yWidth; r++) {
			*(map->data + r*xWidth + c) = -1;
		}
	}

	map->columns = xWidth;
	map->rows = yWidth;

	return map;
}

/**
 * Free all memory for a cgc_Map 
 * @param map_ptr Pointer to the cgc_Map pointer containing the cgc_Map
 */
void cgc_freeMap(cgc_Map **map_ptr) {
	cgc_Map *map;

	map = *map_ptr;
	map->columns = 0;
	map->rows = 0;
	cgc_free(map->data);
	map->data = NULL;
	map->topRight.x = 0;
	map->topRight.y = 0;
	map->bottomLeft.x = 0;
	map->bottomLeft.y = 0;
	cgc_free(map);
	map = NULL;
}

/**
 * Determine if the coordinate (x,y) is within the existing map
 * @param  map The cgc_Map to be tested
 * @param  x   The X-coordinate of the coordinate to find
 * @param  y   The Y-coordinate of the coordinate to find
 * @return     1 if coordinate is in cgc_Map, 0 if not
 */
int cgc_inBounds(cgc_Map* map, int x, int y) {
	if(x < map->bottomLeft.x || x > map->topRight.x || y < map->bottomLeft.y || y > map->topRight.y)
		return 0;

	return 1;
}

/**
 * Make a copy of a cgc_Map from srcMap to dstMap
 * @param destMap The destination of the cgc_Map to copy
 * @param srcMap  The source of the cgc_Map to copy
 */
void cgc_copyMap(cgc_Map* destMap, cgc_Map* srcMap) {
	unsigned int value;

	for(int x=srcMap->bottomLeft.x; x<=srcMap->topRight.x; x++) {
		for(int y=srcMap->bottomLeft.y; y<=srcMap->topRight.y; y++) {
			value = cgc_getCoordinate(srcMap, x, y);
			cgc_setCoordinate(&destMap, x, y, value);
		}
	}
}

/**
 * Add a new coordinate to a cgc_Map.
 * Note: If cgc_Map is not big enough, create a new cgc_Map that is
 * and copy old cgc_Map to new cgc_Map
 * 
 * @param map_ptr [description]
 * @param x       [description]
 * @param y       [description]
 */
void cgc_addCoordinate(cgc_Map** map_ptr, int x, int y) {
	cgc_Coordinate bottomLeft, topRight;
	cgc_Map *nMap, *map;

	map = *map_ptr;
	if(map == NULL) {
		bottomLeft.x = x;
		bottomLeft.y = y;
		topRight.x = x;
		topRight.y = y;
		*map_ptr = cgc_newMap(bottomLeft, topRight);
		return;
	}

	if(cgc_inBounds(map, x,y))
		return;

	if(x < map->bottomLeft.x) {
		bottomLeft.x = x;
		topRight.x = map->topRight.x;
	} else if(x > map->topRight.x) {
		topRight.x = x;
		bottomLeft.x = map->bottomLeft.x;
	} else {
		bottomLeft.x = map->bottomLeft.x;
		topRight.x = map->topRight.x;
	}

	if(y < map->bottomLeft.y) {
		bottomLeft.y = y;
		topRight.y = map->topRight.y;
	} else if(y > map->topRight.y) {
		topRight.y = y;
		bottomLeft.y = map->bottomLeft.y;
	} else {
		bottomLeft.y = map->bottomLeft.y;
		topRight.y = map->topRight.y;
	}

	nMap = cgc_newMap(bottomLeft, topRight);
	cgc_copyMap(nMap, *map_ptr);
	cgc_freeMap(map_ptr);
	*map_ptr = nMap;
}

/**
 * Set the value of a cgc_Coordinate
 * Note: If the coordinate does not exit in the cgc_Map, the coordinate will be added
 * @param map_ptr The address of the cgc_Map containing the cgc_Coordinate
 * @param x       The X-coordinate of the coordinate to set
 * @param y       The Y-coordinate of the coordinate to set
 * @param value   The value to set the coordinate to
 */
void cgc_setCoordinate(cgc_Map** map_ptr, int x, int y, unsigned int value) {
	unsigned int column, row;
	cgc_Map *map;

	if(*map_ptr == NULL || !cgc_inBounds(*map_ptr, x, y))
		cgc_addCoordinate(map_ptr, x, y);

	map = *map_ptr;
	column = x - map->bottomLeft.x + 1;
	row = y - map->bottomLeft.y + 1;

	*(map->data + row*map->columns + column) = value;
}

/**
 * Get the value at a specific coordinate in a cgc_Map
 * Note: All values at valid coordinates should be greater than zero
 * @param  map The cgc_Map to search
 * @param  x   The X-coordinate of the coordinate to find
 * @param  y   The Y-coordinate of the coordinate to find
 * @return     The value at the coordinate, or -1 if the coordinate is not in bounds
 */
unsigned int cgc_getCoordinate(cgc_Map* map, int x, int y) {
	unsigned int column, row;

	if(!cgc_inBounds(map, x, y))
		return -1;

	column = x - map->bottomLeft.x + 1;
	row = y - map->bottomLeft.y + 1;

	return *(map->data + row*map->columns + column);
}
