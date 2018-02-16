#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

#include "ram.h"
#include "wifi.h"
#include "arduino.h"
#include "graphics.h"
#include "touchscreen.h"
#include "gps.h"
#include "image_processor.h"
#include "image_converter.h"
#include "structs.h"
#include "io.h"
#include "main.h"
#include "sys/alt_alarm.h"
#include "ram.h"

// ----------- EXTERN VARIABLES ----------- //
extern rectangle boxes[];

extern colour_t whacky_R[];
extern colour_t whacky_G[];
extern colour_t whacky_B[];
// ----------- GLOBAL VARIABLES ----------- //
// Timestamp
char curr_time[12];

// Serial handling
char gps_buff[256],
	ts_buff[8],
	ard_buff[8];

int gps_inc,
	ard_inc;

int gps_ready,
	ts_ready;

// Touchscreen states
int ts_state,
	prev_ts_state,
	curr_btn,
	stop_btn_pressed;
mode_state curr_mode;

// Autosort state
sort_state curr_sort;
int image_scanned;
int colour_scanned;

// Scanned objects
scanned_obj* red_object;
scanned_obj* green_object;
scanned_obj* blue_object;
scanned_obj* other_object;

int main() {
	printf("Initializing...\n");

	// Initialize loop variables	
	curr_time[0] = '\0';


	ts_state = 0;
	prev_ts_state = TS_STATE_UNTOUCHED;
	curr_btn = -1;
	stop_btn_pressed = 0;
	curr_mode = MODE_IDLE;

	gps_inc = 0;
	ard_inc = 0;

	gps_ready = 0;
	ts_ready = 0;

	curr_sort = SORT_IDLE;
	image_scanned = 0;
	colour_scanned = 0;

	red_object = init_scanned_obj(RED, RED_POS, RED_OBJ_LOC);
	green_object = init_scanned_obj(LIME, GREEN_POS, GREEN_OBJ_LOC);
	blue_object = init_scanned_obj(BLUE, BLUE_POS, BLUE_OBJ_LOC);
	other_object = init_scanned_obj(BLACK, OTHER_POS, OTHER_OBJ_LOC);

	scanned_obj* objects[4] = {
		red_object,
		green_object,
		blue_object,
		other_object
	};

	// Initialiize modules
	init_touch();
	init_gps();
	init_arduino();
	init_wifi();

	// Reset GUI
	clear_screen();
	draw_screen();

	srand(time(NULL));

	// TEST IMAGE START
	int size_x = 160;
	int size_y = 128;
	int res = size_x * size_y;

	printf("Ready!\n");
	// Main loop
	while (1) {
		// Handle module Rx inputs
		poll_gps();
		poll_touchscreen();
		poll_arduino();
		
		// Check if any module has finished sending a command
		if (gps_ready) {
			handle_gps();
		}
		if (ts_ready) {
			handle_touchscreen();
		}
		// Auto sort process
		if (curr_mode == MODE_AUTO_SORT) {
			// Check if Arduino has detected object in laser or has timed out
			if (curr_sort == SORT_ARD_READY) {
				handle_arduino();
			}
			// If object detected, capture image
			else if (curr_sort == SORT_CAM_READY) {
				// Signify that an image is being taken
				colour_scanned = rand() % 4 + 2;
				int ImageColor[res];
				int x;
				for (x = 0; x < res; x++) {
					ImageColor[x] = colour_scanned;
				}
				char tmp[160*128/8];
				int q = 0;
				// Slight delay in taking image
				usleep(500000);
				// Read image taken from SDRAM
				*(RAMControl) = 0b000;
				for (; q < 160*128/8; q++){
					int r = 0;
					char c = 0;
					for (; r < 8; r++){
						int pixel = *(RAMStart + q*8 + r);
						//printf("%x\n", pixel);
						int red = (pixel >> 5) & 0b111;
						int green = (pixel >> 2) & 0b111;
						int blue = pixel & 0b11;
						//printf("R: %d, G: %d, B: %d\n", red, green, blue);
						int sum = red + green + blue;
						int turn_on = 0;
						if (sum > 8) {
							turn_on = 1;
						}
						c |= turn_on << (7 - r);
					}
					tmp[q] = c;
				}
				*(RAMControl) = 0b100;
				//Draw image on screen
				OutGraphicsImage(IMG_LOC.x, IMG_LOC.y, size_x, size_y, tmp, ImageColor);
				curr_sort = SORT_IMG_READY;
			}
			// Once image has been captured, process image
			else if (curr_sort == SORT_IMG_READY) {
				// Update the image category
				scanned_obj* obj = objects[colour_scanned-2];
				// update and draw new object count
				draw_counter(obj->loc, ++obj->count);
				// set flag to draw timestamp at time scanned
				image_scanned = 1;
				// set direction
				set_servo(obj->pos);
				// allow servo to reach a position
				usleep(500000);
				// run conveyor
				conveyor(ON);
				// allow object to be dumped into a bin before sort cycle is run
				usleep(500000);
				// run sort cycle again
				auto_sort();
				// wait for next object to arrive
				curr_sort = SORT_IDLE;
			}
		}
	}
	return 0;
}

// Poll for GPS data
void poll_gps() {
	if(is_gps_data_ready()) {
		char c = get_char_gps();
		gps_buff[gps_inc++] = c;
		if(c == '\n') {
			gps_ready = 1;
		}
	}
}

// Poll for Touchscreen data
// Update state if event has touched
// eg. touch to release or new touch
void poll_touchscreen() {
	if (is_screen_touched()) {
		int c = get_char_touchscreen();
		if (c == TS_PRESS_EVENT) {
			ts_state = TS_STATE_TOUCHED;
		}
		else if (c == TS_RELEASE_EVENT) {
			ts_state = TS_STATE_UNTOUCHED;
		}
		else {
			return; 
		}
		int i;
		for (i = 0; i < 4; i++){
			ts_buff[i] = get_char_touchscreen();
		}
		ts_ready = 1;
	}
}

// Poll for Arduino data
void poll_arduino() {
	if (is_arduino_data_ready()) {
		char c = get_char_arduino();
		ard_buff[ard_inc++] = c;
		if (c == '\n') {
			curr_sort = SORT_ARD_READY;
		}
	}
}

/*
 * Fetches timestamp from GPS and updates the value on the screen
 * if the timestamp is different
 */
void handle_gps() {
	char new_time[12] = "";
	parse_gps_buffer(gps_buff, new_time);
	// if the timestamps are different, redraw the time
	if (new_time[0] != '\0' && strcmp(curr_time, new_time) != 0) {
		// save new time
		strcpy(curr_time, new_time);
		// draw new time
		WriteStringFont2(CURR_TIME_LOC.x, CURR_TIME_LOC.y, BLACK, BG_COLOUR, curr_time, 1);
		// if camera has taken a picture, write time under image
		if (image_scanned) {
			WriteStringFont2(SCAN_TIME_LOC.x, SCAN_TIME_LOC.y, BLACK, BG_COLOUR, curr_time, 1);
			image_scanned = 0;
		}
	}	
	gps_inc = gps_ready = 0;
	strcpy(gps_buff, "");
}

/*
 * Performs button actions based on where the touch input is placed.
 */
void handle_touchscreen() {
	int is_changed = 0;
	if (prev_ts_state != ts_state) {
		is_changed = 1;
		prev_ts_state = ts_state;
	}
	if (is_changed == 1) {
		// get the point of touch
		int x = (((int)ts_buff[1]) << 7) + ((int)ts_buff[0]);
		int y = (((int)ts_buff[3]) << 7) + ((int)ts_buff[2]);
		point p = get_calibrated_point(x, y);

		if (ts_state == TS_STATE_TOUCHED) {
			// special case for touch handle during auto sorting or override
			if (curr_mode == MODE_AUTO_SORT || curr_mode == MODE_OVERRIDE) {
				// Terminates auto sort or override processes
				if (touch_in_button(p, STOP_BTN)) {
					stop_btn_pressed = 1;
					curr_mode = MODE_IDLE;
					conveyor(OFF);
				}
			}
			else if (curr_mode != MODE_SWEEP) {
				// Reset object counters and begin auto sort process
				if (touch_in_button(p, AUTO_SORT_BTN)) {
					curr_btn = 0;
					curr_mode = MODE_AUTO_SORT;
					red_object->count = green_object->count = blue_object->count = other_object->count = 0;
					reset_counters();
					auto_sort();
				}
				// SWEEP_CW and SWEEP_CCW sweep the servo in it's respective direction
				else if (touch_in_button(p, SWEEP_CW_BTN)) {
					curr_btn = 2;
					curr_mode = MODE_SWEEP;
					sweep(CW);
				}
				else if (touch_in_button(p, SWEEP_CCW_BTN)) {
					curr_btn = 3;
					curr_mode = MODE_SWEEP;
					sweep(CCW);
				}
				// POS_1 to POS_4 buttons are overrides. Sets servo position and runs conveyor
				else if (touch_in_button(p, POS_1_BTN)) {
					curr_btn = 4;
					curr_mode = MODE_OVERRIDE;
					set_servo(RED_POS);
					conveyor(ON);
				}
				else if (touch_in_button(p, POS_2_BTN)) {
					curr_btn = 5;
					curr_mode = MODE_OVERRIDE;
					set_servo(GREEN_POS);
					conveyor(ON);
				}
				else if (touch_in_button(p, POS_3_BTN)) {
					curr_btn = 6;
					curr_mode = MODE_OVERRIDE;
					set_servo(BLUE_POS);
					conveyor(ON);
				}
				else if (touch_in_button(p, POS_4_BTN)) {
					curr_btn = 7;
					curr_mode = MODE_OVERRIDE;
					set_servo(OTHER_POS);
					conveyor(ON);
				}
			}
			// No button is pressed
			else {
				curr_btn = -1;
			}

			// If pressing a button, bold the pressed button
			if (curr_btn > -1 && !stop_btn_pressed) {
				draw_rectangle(boxes[curr_btn], BOLDED);
			}
			else if (stop_btn_pressed) {
				draw_rectangle(boxes[1], BOLDED);
			}
		}
		else {
			// once touch has been released, stop sweeping
			if (curr_mode == MODE_SWEEP) {
				curr_mode = MODE_IDLE;
				sweep(STOP);
			}

			// redraw rectangle if releasing a sweep button
			if (curr_btn == 2 || curr_btn == 3) {
				reset_button(boxes[curr_btn]);
				curr_btn = -1;
			}
			// special case for AutoSort and STOP buttons
			else if (stop_btn_pressed) {
				reset_button(boxes[curr_btn]);
				reset_button(boxes[1]);
				stop_btn_pressed = 0;
				curr_btn = -1;
			}
		}
	}
	// clear ready flag and buffer
	ts_ready = 0;
	strcpy(ts_buff, "");
}

/*
 * Check if the Arduino has returned a serial command
 * ls: laser has been triggered,
 * 		signifying an object is ready to be scanned
 * dn: timeout has been triggered,
 * 		send a text notifying user the scanning process has been completed
 */
void handle_arduino() {
	// laser has been broken, prepare to capture object
	if (strcmp(ard_buff, "ls\n") == 0) {
		curr_sort = SORT_CAM_READY;
	}
	// timeout has been reached, stop process and send completion text
	else if (strcmp(ard_buff, "dn\n") == 0) {
		char text[256];
		sprintf(text, "Sorting complete!\\\nResults - Red: %i   Green: %i   Blue: %i   Other: %i",
			red_object->count, green_object->count, blue_object->count, other_object->count);
		printf("%s\n", text);
		send_text(text);
		printf("Succesfully alerted\n", text);

		// reset GUI and states
		reset_button(boxes[curr_btn]);
		reset_button(boxes[1]);
		curr_btn = -1;
		curr_sort = SORT_IDLE;
		curr_mode = MODE_IDLE;		
	}
	else {
		curr_sort = SORT_IDLE;
	}
	// clear ready flag and buffer
	ard_inc = 0;
	strcpy(ard_buff, "");
}

/*
 * Helper to initialize scanned object structs.
 */
scanned_obj* init_scanned_obj(int colour, int position, point location) {
	scanned_obj* obj = malloc(sizeof(scanned_obj));

	obj->colour = colour;
	obj->pos = position;
	obj->loc = location;
	obj->count = 0;

	return obj;
}
