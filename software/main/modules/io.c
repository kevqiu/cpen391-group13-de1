#include <string.h>
#include "io.h"

extern int SevenSegmentASCII[];

void display_hex(char* string) {
	int i = strlen(string)-1;
	int hex_inc = 0;

	// map char to seven segment display
	while (i >= 0) {
		char c = string[i--];
		int seg_bits = ~SevenSegmentASCII[c-32];
		set_display(seg_bits, hex_inc++);
	}
	// set the rest of the displays to be empty
	while (hex_inc < 6) {
		set_display(0b1111111, hex_inc++);
	}
}

void set_display(int seg_bits, int segment) {
	switch(segment) {
		case 0:
			HEX_0 = seg_bits;
			break;
		case 1:
			HEX_1 = seg_bits;
			break;
		case 2:
			HEX_2 = seg_bits;
			break;
		case 3:
			HEX_3 = seg_bits;
			break;
		case 4:
			HEX_4 = seg_bits;
			break;
		case 5:
			HEX_5 = seg_bits;
			break;
		default:
			break;
	}
}
