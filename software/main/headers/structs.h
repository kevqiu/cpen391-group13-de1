#ifndef _STRUCTS_
#include <inttypes.h>
#define _STRUCTS_

typedef uint16_t colour_t;

// GUI Structs
typedef struct {
	int x1, x2, y1, y2;
	int colour;
} rectangle;

typedef struct {
	int x, y;
	int colour;
	char* text;
} text;

typedef struct {
	int x, y, len;
	int colour;
} line;

typedef struct {
	int x, y;
} point;

typedef struct {
	int colour;
	int pos;
	int count;
	point loc;
} scanned_obj;

typedef struct {
	uint16_t r, g, b;
} rgb_t;

typedef struct {
	int colour;
	colour_t* colour_data;
} image_t;

typedef struct {
	int colour;
	int* colour_data;
} image8_t;



#endif
