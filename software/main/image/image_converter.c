#include <math.h>
#include <limits.h>
#include <inttypes.h>
#include "image_converter.h"

extern unsigned int ColourPaletteData[];

/*
 * Converts an image in 16 bit colour space to an image to be
 * consumed by OutGraphics.
 * input: array in 16 bit colour space, from memory
 * output: array compatible with programmed colour palette
 */
void map_image_to_palette(colour_t* input, int* output, int size) {
    int i;
    for (i = 0; i < size; i++) {
        output[i] = map_rgb_to_palette(convert_16_bit_to_rgb(input[i]));
    }
}

int map_rgb_to_palette(rgb_t val) {
    int closest_colour = 1; // default value is white;
    int min_dist = INT_MAX;

    // iterate through each palette colour, find colour closest to value
    int i;
    for (i = 0; i < PALETTE_SIZE; i++) {
        rgb_t p = convert_hex_to_rgb(ColourPaletteData[i]);
        int dist = sqrt(pow(val.r - p.r, 2) + // red dist 
                        pow(val.g - p.g, 2) + // green dist
                        pow(val.b - p.b, 2)); // blue dist
        if (dist < min_dist) {
            closest_colour = i;
            min_dist = dist;
        }
    }
    return closest_colour;
}

void convert_8_bit_to_16_bit(int* r, int* g, int* b, colour_t* img, int size) {
	int i;
	for(i = 0; i < size; i++) {
		int red = ((r[i] >> 3) & 0b00011111);
		int green = ((g[i] >> 2) & 0b00111111);
		int blue = ((b[i] >> 3) & 0b00011111);
        colour_t new_val = (red << 11) | (green << 5) | (blue);
        img[i] = new_val;
	}
}

/*
 * Converts hex colour space into consumable RGB values.
 * 0xRRGGBB
 */
rgb_t convert_hex_to_rgb(int value) {
    rgb_t rgb;
    rgb.r = value >> 16 & 0xFF;
    rgb.g = value >> 8 & 0xFF;
    rgb.b = value & 0xFF;
    return rgb;
}

/*
 * Converts 16 bit colour space into consumable RGB values.
 * 0bRRRRRGGGGGGBBBBB
 */
rgb_t convert_16_bit_to_rgb(colour_t value) {
    rgb_t rgb;
    rgb.r = (value >> 11 & 0b11111) << 3;
    rgb.g = (value >> 5 & 0b111111) << 2;
    rgb.b = (value & 0b11111) << 3;
    return rgb;
}
