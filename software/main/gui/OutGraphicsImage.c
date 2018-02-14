#include "graphics.h"

void OutGraphicsImage(int x, int y, int x_size, int y_size, char* img, int* colour_map)
{
	register int row = 0,
				col = 0,
				bit = 0,
				pixels;
	register int BitMask;

	for(row = 0; row < y_size; row++) {
		for(col = 0; col < x_size / 8; col++) {
			pixels = img[(row * x_size / 8) + col];		     	// get the pixels for row 0 of the character to be displayed
			BitMask = 0b10000000;
			for(bit = 0; bit < 8; bit++) {
				if((pixels & BitMask))							// if valid pixel, then write it
					WriteAPixel(x + (col * 8) + bit,
						y + row, 
						colour_map[(row * x_size) + (col * 8) + bit]);
				else {
					WriteAPixel(x + (col * 8) + bit,
						y+row,
						BLACK);
				}
				BitMask = BitMask >> 1;
			}
		}
	}

}
