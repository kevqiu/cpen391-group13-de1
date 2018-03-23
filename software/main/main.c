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
#include "rpi.h"
#include "image_processor.h"
#include "image_converter.h"
#include "structs.h"
#include "io.h"
#include "main.h"
#include "sys/alt_alarm.h"
#include "ram.h"

// ----------- EXTERN VARIABLES ----------- //
extern rectangle boxes[];
extern int SevenSegmentASCII[];

// ----------- GLOBAL VARIABLES ----------- //
// Timestamp
char* curr_time = "00:00:00";
char gpgga_sentence[128] = {0};

// Serial handling
char gps_buff[256],
	ts_buff[8],
	ard_buff[8],
	rpi_buff[16];

int gps_inc,
	ard_inc,
	rpi_inc;

int gps_ready,
	ts_ready,
	rpi_ready;

// Touchscreen states
int ts_state,
	prev_ts_state,
	curr_btn,
	stop_btn_pressed;
mode_state curr_mode;

// Autosort state
sort_state curr_sort;
int image_scanned;
int category_scanned;
int cycle_id;

// Scanned objects
scanned_obj* red_object;
scanned_obj* green_object;
scanned_obj* blue_object;
scanned_obj* other_object;

// Mock location
int last_switch;
int mock_location;
char* mock_lat;
char* mock_lng;

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
	rpi_ready = 0;

	curr_sort = SORT_IDLE;
	image_scanned = 0;
	category_scanned = 0;
	cycle_id = 0;

	red_object = init_scanned_obj(RED, RED_POS, RED_OBJ_LOC);
	green_object = init_scanned_obj(LIME, GREEN_POS, GREEN_OBJ_LOC);
	blue_object = init_scanned_obj(BLUE, BLUE_POS, BLUE_OBJ_LOC);
	other_object = init_scanned_obj(BLACK, OTHER_POS, OTHER_OBJ_LOC);

	last_switch = -1;
	mock_location = 0;

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
	init_rpi();

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
		check_switches();

		// Handle module Rx inputs
		poll_gps();
		poll_touchscreen();
		poll_arduino();
		poll_rpi();
		
		// Check if any module has finished sending a command
		if (gps_ready) {
			handle_gps();
		}
		if (ts_ready) {
			handle_touchscreen();
		}
		if (rpi_ready) {
			handle_rpi();
		}

		// Auto sort process
		if (curr_mode == MODE_AUTO_SORT) {
			// send message to initialize autosort and get cycle
			if (curr_sort == SORT_INIT) {
				initialize_autosort(curr_time);
				curr_sort = SORT_INIT_WAIT;
			}
			// turn on machinery			
			else if (curr_sort == SORT_BEGIN_CYCLE) {
				auto_sort();
				curr_sort = SORT_ARD_WAIT;
			}
			// Check if Arduino has detected object in laser or has timed out
			else if (curr_sort == SORT_ARD_READY) {
				handle_arduino();
			}
			// If object detected, capture image
			else if (curr_sort == SORT_CAM_READY) {
				draw_silhouette(size_x, size_y);
				capture_image(gpgga_sentence);
				curr_sort = SORT_IMG_WAIT;
			}
			// Once image has been captured, process image
			else if (curr_sort == SORT_IMG_READY) {
				// Update the image category, subtract 1 to offset id index starting at 1
				scanned_obj* obj = objects[category_scanned - 1];
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
				curr_sort = SORT_INIT;
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

// Poll for Raspberry Pi data
void poll_rpi() {
	if (is_rpi_data_ready()) {
		char c = get_char_rpi();
		rpi_buff[rpi_inc++] = c;
		if (c == '\r') {
			rpi_ready = 1;
		}
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
	if (strstr(gps_buff, GPGGA_COMMAND) != NULL) {
		strcpy(gpgga_sentence, gps_buff);
		gpgga_sentence[strlen(gpgga_sentence) - 2] = '\0';

		if (mock_location) {
			char mock_string[64];
			sprintf(mock_string, " mock_lat=%s mock_lng=%s", mock_lat, mock_lng);
			strcat(gpgga_sentence, mock_string);
		}
	}
	printf("%s\n", gpgga_sentence);

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
	memset(gps_buff, 0, 256);
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
					set_mode(MODE_IDLE, 1);
				}
			}
			else if (curr_mode != MODE_SWEEP) {
				// Reset object counters and begin auto sort process
				if (touch_in_button(p, AUTO_SORT_BTN)) {
					curr_btn = 0;
					set_mode(MODE_AUTO_SORT, 0);
				}
				// SWEEP_CW and SWEEP_CCW sweep the servo in it's respective direction
				else if (touch_in_button(p, SWEEP_CW_BTN)) {
					curr_btn = 2;
					set_mode(MODE_SWEEP, CW);
				}
				else if (touch_in_button(p, SWEEP_CCW_BTN)) {
					curr_btn = 3;
					set_mode(MODE_SWEEP, CCW);
				}
				// POS_1 to POS_4 buttons are overrides. Sets servo position and runs conveyor
				else if (touch_in_button(p, POS_1_BTN)) {
					curr_btn = 4;
					set_mode(MODE_OVERRIDE, RED_POS);
				}
				else if (touch_in_button(p, POS_2_BTN)) {
					curr_btn = 5;
					set_mode(MODE_OVERRIDE, GREEN_POS);
				}
				else if (touch_in_button(p, POS_3_BTN)) {
					curr_btn = 6;
					set_mode(MODE_OVERRIDE, BLUE_POS);
				}
				else if (touch_in_button(p, POS_4_BTN)) {
					curr_btn = 7;
					set_mode(MODE_OVERRIDE, OTHER_POS);
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
	memset(ts_buff, 0, 8);
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
		// trigger FCM notification on server and save cycle end time
		sprintf(text, "done:time=%s,cycle_id=%i", curr_time, cycle_id);
		send_message_rpi(text);
		printf("Succesfully alerted\n");

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
	memset(ard_buff, 0, 8);
}

/*
 * Check if the Raspberry Pi has returned a serial message
 * cat: 		object has been identified on the server and has been stored
 * ctrl/as=:  	enable/disable autosort
 * ctrl/pos=: 	override to position
 */
void handle_rpi() {
	printf("Pi: %s\n", rpi_buff);
	if (strstr(rpi_buff, "cycle_id=") != NULL) {
		// get cycle id substring from message
		char sub[4];
		memcpy(sub, rpi_buff + 9, strlen(rpi_buff) - 9);
		sub[strlen(rpi_buff)-1] = '\0';
		cycle_id = atoi(sub);

		curr_sort = SORT_BEGIN_CYCLE;
	}
	else if (strstr(rpi_buff, "cat:") != NULL) {
		category_scanned = rpi_buff[4] - '0';
		curr_sort = SORT_IMG_READY;
	}
	else if (strstr(rpi_buff, "ctrl/as") != NULL) {
		set_mode(MODE_AUTO_SORT, 1);
	}
	else if (strstr(rpi_buff, "ctrl/st") != NULL) {
		set_mode(MODE_IDLE, 1);
	}
	else if (strstr(rpi_buff, "ctrl/pos=") != NULL) {
		int position = rpi_buff[9] - '0';
		if (position == 1) {
			set_mode(MODE_OVERRIDE, RED_POS);
		}
		else if (position == 2) {
			set_mode(MODE_OVERRIDE, GREEN_POS);
		}
		else if (position == 3) {
			set_mode(MODE_OVERRIDE, BLUE_POS);
		}
		else if (position == 4) {
			set_mode(MODE_OVERRIDE, OTHER_POS);
		}
	}
	rpi_inc = rpi_ready = 0;
	memset(rpi_buff, 0, 16);
}

void set_mode(mode_state state, int value) {
	switch(state) {
		case MODE_AUTO_SORT:
			curr_mode = MODE_AUTO_SORT;
			curr_sort = SORT_INIT;
			red_object->count = green_object->count = blue_object->count = other_object->count = 0;
			reset_counters();
			break;

		case MODE_SWEEP:
			curr_mode = MODE_SWEEP;
			sweep(value);
			break;

		case MODE_OVERRIDE:
			curr_mode = MODE_OVERRIDE;
			set_servo(value);
			conveyor(ON);
			break;

		case MODE_IDLE:
			// stop command has been received, check if it was during Autosort
			if (curr_mode == MODE_AUTO_SORT) {
				sprintf(text, "done:time=%s,cycle_id=%i", curr_time, cycle_id);
				send_message_rpi(text);
			}
			curr_mode = MODE_IDLE;
			conveyor(OFF);
			break;

		default:
			break;
	}
}

void draw_silhouette(int size_x, int size_y) {
	int res = size_x * size_y;
	// Signify that an image is being taken
	int ImageColor[res];
	int x;
	for (x = 0; x < res; x++) {
		ImageColor[x] = BLACK;
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
}

void check_switches() {
	// Mock our location, use last_switch to prevent cycling of this switch case
	if (last_switch != switches) {
		// debounce delay
		usleep(50000);
		switch (switches) {
			case 0:
				display_hex(LOCATION_UBC);
				mock_location = 0;
				break;
			case 1:
				display_hex(LOCATION_UOFT);
				mock_location = 1;
				mock_lat = LAT_UOFT;
				mock_lng = LNG_UOFT;
				break;
			case 2:
				display_hex(LOCATION_MCGILL);
				mock_location = 1;
				mock_lat = LAT_MCGILL;
				mock_lng = LNG_MCGILL;
				break;
			default:
				break;
		}
		last_switch = switches;
	}
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
