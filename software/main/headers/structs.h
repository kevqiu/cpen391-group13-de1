#ifndef _STRUCTS_

#define _STRUCTS_

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

#endif
