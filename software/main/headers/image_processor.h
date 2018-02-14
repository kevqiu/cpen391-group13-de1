#ifndef _STRUCTS_
#include "structs.h"
#endif

#define LOWER_THRESHOLD 150

image_t* process_image(colour_t* img, int res);
colour_t avg(colour_t* arr, int arr_len, int count);
char relevant_pixels(colour_t* col, int offset);
