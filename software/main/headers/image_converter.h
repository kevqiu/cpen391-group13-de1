#ifndef _STRUCTS_
#include "structs.h"
#endif

#define PALETTE_SIZE 6

void map_image_to_palette(colour_t* input, int* output, int size);
void convert_8_bit_to_16_bit(colour_t* r, colour_t* g, colour_t* b, colour_t* img, int size);
int map_rgb_to_palette(rgb_t val);
rgb_t convert_hex_to_rgb(int value);
rgb_t convert_16_bit_to_rgb(colour_t value);
