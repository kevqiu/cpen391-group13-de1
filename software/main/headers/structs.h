#ifndef _STRUCTS_
#define _STRUCTS_

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
	int x, y;
} point;

#endif
