#ifndef _STRUCTS_
#include "structs.h"
#endif

#define LOWER_THRESHOLD 150
#define LOWER_8_BIT_THRESHOLD 5

image_t* process_image(colour_t* img, int res);
colour_t avg(colour_t* arr, int arr_len, int count);
char relevant_pixels(colour_t* col, int offset);
void calculate_relevant_pixels(colour_t* in, char* pixels, int res);
void calculate_relevant_8_bit_pixels(int* in, char* pixels, int res);
image8_t* process_8_bit_image(int* img, int res);
