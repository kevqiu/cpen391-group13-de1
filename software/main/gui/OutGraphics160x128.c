#include "graphics.h"


#define IMAGE_XPIXELS	160				// width of Font2 characters in pixels (no spacing)
#define IMAGE_YPIXELS	128				// height of Font2 characters in pixels (no spacing)

void OutGraphics160x128(int x, int y, char* img)
{
	register int row = 0,
				col = 0,
				bit = 0,
				pixels;
	register int BitMask;

//    if(((short)(x) > (short)(XRES-1)) || ((short)(y) > (short)(YRES-1)))  // if start off edge of screen don't bother
//        return ;

	for(row = 0; row < IMAGE_YPIXELS; row++) {
		for(col = 0; col < IMAGE_XPIXELS/8; col++) {
			pixels = img[(row*IMAGE_XPIXELS/8) + col];		     	// get the pixels for row 0 of the character to be displayed
			BitMask = 0b10000000;
			for(bit = 0; bit < 8; bit++) {
				if((pixels & BitMask))							// if valid pixel, then write it
					WriteAPixel(x+(col*8)+bit, y+row, BLACK) ;
				else {
					WriteAPixel(x+(col*8)+bit, y+row, WHITE);
				}
				BitMask = BitMask >> 1;
			}
		}
	}

}
