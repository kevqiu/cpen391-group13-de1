#ifndef _STRUCTS_
#include "structs.h"
#endif

#define LOWER_BOUND 15
#define UPPER_BOUND 180

image_t* process_image(colour_t* img, int res);
colour_t avg(colour_t* arr, int count);
char relevant_pixels(colour_t* col, int avg_val, int offset);
