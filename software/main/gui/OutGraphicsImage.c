#include "graphics.h"

/*
 * Displays an image of x_size width and y_size height at position (x,y).
 * Inputs:
 * 	 x: x coordinate image starts
 * 	 y: y coordinate image starts
 * 	 x_size: width of image
 * 	 y_size: height of image
 *   img: matrix containing chars which represent the status of a bit in the image.
 * 	 	 1 = draw pixel, 0 = don't draw
 *   colour_map: matrix containing colours corresponding to palette
 * Output:
 * 	 image is drawn on the GUI at position (x,y)
 */
void OutGraphicsImage(int x, int y, int x_size, int y_size, char* img, int* colour_map)
{
	register int row = 0,
				col = 0,
				bit = 0,
				pixels;
	register int BitMask;

	for (row = 0; row < y_size; row++) {
		for (col = 0; col < x_size / 8; col++) {
			pixels = img[(row * x_size / 8) + col];		     	
			BitMask = 0b10000000;
			for (bit = 0; bit < 8; bit++) {
				// if valid pixel, then write it
				if (pixels & BitMask)							
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
