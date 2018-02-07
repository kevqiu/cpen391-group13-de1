#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "wifi.h"
#include "arduino.h"
#include "rs232.h"
#include "graphics.h"
#include "touchscreen.h"
#include "gps.h"
#include "structs.h"
#include "io.h"
#include "main.h"

// ----------- EXTERN VARIABLES ----------- //
extern rectangle boxes[];

extern char Trump[];
extern char Pepe[];

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
	curr_btn;
mode_state curr_mode;

// Autosort state
sort_state curr_sort;
int image_scanned;

// Object counts
int red_obj_count,
	green_obj_count,
	blue_obj_count,
	other_obj_count;

// ---------- FUNCTION PROTOTYPES --------- //
void poll_gps(void);
void handle_gps(void);

void poll_touchscreen(void);
void handle_touchscreen(void);

void poll_arduino(void);
void handle_arduino(void);

int main() {
	printf("Initializing...\n");

	// Initialize loop variables	
	curr_time[0] = '\0';

	ts_state = 0;
	prev_ts_state = TS_STATE_UNTOUCHED;
	curr_btn = -1;
	curr_mode = MODE_IDLE;

	gps_inc = 0;
	ard_inc = 0;

	gps_ready = 0;
	ts_ready = 0;

	curr_sort = SORT_IDLE;
	image_scanned = 0;

	red_obj_count = 0;
	green_obj_count = 0;
	blue_obj_count = 0;
	other_obj_count = 0;

	// Initialiize modules
	init_touch();
	init_gps();
	init_arduino();

	// Reset GUI
	clear_screen();
	draw_screen();

	srand(time(NULL));
	int x;
	int ImageColor[160*128];
	for (x = 0; x < 160*128; x++) {
		ImageColor[x] = rand() % 7 + 1;
	}
	
	OutGraphicsImage(IMG_LOC.x, IMG_LOC.y, 160, 128, Trump, ImageColor);

	printf("Ready!\n");
	
	// temp
	int btn_lock = 0;

	// Main loop
	while (1) {
		poll_gps();
		poll_touchscreen();
		poll_arduino();

		if (gps_ready) {
			handle_gps();
		}

		if (ts_ready) {
			handle_touchscreen();
		}

		if (curr_mode == MODE_AUTO_SORT) {
			if (curr_sort == SORT_ARD_READY) {
				handle_arduino();
			}

			else if (curr_sort == SORT_CAM_READY) {
				// "take a picture" using a button
				if (push_buttons & 0b100) {
					curr_sort = SORT_IMG_READY;
				}
			}

			else if (curr_sort == SORT_IMG_READY) {
				// process image

				// determine object
				int img = rand() % 4;
				int colour;
				int servo_pos = 0;

				// update object count
				if (img == 0) {
					colour = RED;
					servo_pos = RED_POS;
					red_obj_count++;
					draw_counter(RED_OBJ_LOC, red_obj_count);
				}
				else if (img == 1) {
					colour = LIME;
					servo_pos = GREEN_POS;
					green_obj_count++;
					draw_counter(GREEN_OBJ_LOC, green_obj_count);
				}
				else if (img == 2) {
					colour = BLUE;
					servo_pos = BLUE_POS;
					blue_obj_count++;
					draw_counter(BLUE_OBJ_LOC, blue_obj_count);
				}
				else if (img == 3) {
					colour = BLACK;
					servo_pos = OTHER_POS;
					other_obj_count++;
					draw_counter(OTHER_OBJ_LOC, other_obj_count);
				}

				// draw new object count


				// draw image on screen
				for (x = 0; x < 160*128; x++) {
					ImageColor[x] = colour;
				}
				OutGraphicsImage(IMG_LOC.x, IMG_LOC.y, 160, 128, Trump, ImageColor);

				// set flag to draw timestamp at time scanned
				image_scanned = 1;

				// set direction
				set_servo(servo_pos);

				// run conveyor cycle again
				auto_sort();

				// wait for next object to hit limit switch
				curr_sort = SORT_IDLE;
			}
		}

		// if (curr_mode == MODE_AUTO_SORT) {
		// 	if (btn_lock == 0) {
		// 		if (push_buttons & 0b100) {
		// 			red_obj_count++;
		// 			draw_counter(RED_OBJ_LOC, red_obj_count);
		// 			btn_lock = 1;

		// 			for (x = 0; x < 160*128; x++) {
		// 				ImageColor[x] = RED;
		// 			}
		// 			OutGraphicsImage(IMG_LOC.x, IMG_LOC.y, 160, 128, Trump, ImageColor);
		// 		}
		// 		else if (push_buttons & 0b010) {
		// 			green_obj_count++;
		// 			draw_counter(GREEN_OBJ_LOC, green_obj_count);
		// 			btn_lock = 1;

		// 			for (x = 0; x < 160*128; x++) {
		// 				ImageColor[x] = LIME;
		// 			}
		// 			OutGraphicsImage(IMG_LOC.x, IMG_LOC.y, 160, 128, Trump, ImageColor);
		// 		}
		// 		else if (push_buttons & 0b001) {
		// 			blue_obj_count++;
		// 			draw_counter(BLUE_OBJ_LOC, blue_obj_count);
		// 			btn_lock = 1;

		// 			for (x = 0; x < 160*128; x++) {
		// 				ImageColor[x] = BLUE;
		// 			}
		// 			OutGraphicsImage(IMG_LOC.x, IMG_LOC.y, 160, 128, Trump, ImageColor);
		// 		}
		// 	}
		// 	else {
		// 		btn_lock = push_buttons > 0;
		// 	}
		// }
	}

	printf("Complete!\n");
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
	if(is_arduino_data_ready()) {
		char c = get_char_arduino();
		ard_buff[ard_inc++] = c;
		if(c == '\n') {
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
	//insert_time_colons(new_time);
	// if the timestamps are different, redraw the time
	if(new_time[0] != '\0' && strcmp(curr_time, new_time) != 0) {
		// save new time
		strcpy(curr_time, new_time);
		// draw new time
		WriteStringFont2(CURR_TIME_LOC.x, CURR_TIME_LOC.y, BLACK, BG_COLOUR, curr_time, 1);
		
		// if camera has taken a picture, write time under image
		if(image_scanned) {
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
			// special case for touch handle during auto sorting
			// only button that can pressed is STOP 
			if (curr_mode == MODE_AUTO_SORT) {
				if (touch_in_button(p, STOP_BTN)) {
					curr_btn = 1;
					curr_mode = MODE_IDLE;
				}
			}
			else if (curr_mode != MODE_SWEEP) {
				if (touch_in_button(p, AUTO_SORT_BTN)) {
					curr_btn = 0;
					curr_mode = MODE_AUTO_SORT;
					auto_sort();
				}
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
				else if (touch_in_button(p, POS_1_BTN)) {
					curr_btn = 4;
					set_servo(RED_POS);
				}
				else if (touch_in_button(p, POS_2_BTN)) {
					curr_btn = 5;
					set_servo(GREEN_POS);
				}
				else if (touch_in_button(p, POS_3_BTN)) {
					curr_btn = 6;
					set_servo(BLUE_POS);
				}
				else if (touch_in_button(p, POS_4_BTN)) {
					curr_btn = 7;
					set_servo(OTHER_POS);
				}
			}
			else {
				curr_btn = -1;
			}

			if(curr_btn > -1) {
				draw_rectangle(boxes[curr_btn], BOLDED);
			}
		}
		else {
			// once touch has been released, stop sweeping
			if (curr_mode == MODE_SWEEP) {
				curr_mode = MODE_IDLE;
				sweep(STOP);
			}
			
			// redraw rectangle if not touching a button
			if (curr_btn > 1) {
				clear_bolded_rectangle(boxes[curr_btn]);
				draw_rectangle(boxes[curr_btn], EMPTY);
				curr_btn = -1;
			}
			// special case for AutoSort and STOP buttons
			else if (curr_btn == 1) {
				clear_bolded_rectangle(boxes[0]);
				clear_bolded_rectangle(boxes[1]);
				draw_rectangle(boxes[0], EMPTY);
				draw_rectangle(boxes[1], EMPTY);
				curr_btn = -1;
			}
		}
	}

	ts_ready = 0;
	strcpy(ts_buff, "");
}

/*
 * Check if the Arduino has returned a serial command
 * ls: limit switch has been triggered,
 * 		signifying an object is ready to be scanned
 */
void handle_arduino() {
	if (strcmp(ard_buff, "ls\n") == 0) {
		curr_sort = SORT_CAM_READY;
	}
	else {
		curr_sort = SORT_IDLE;
	}

	ard_inc = 0;
	strcpy(ard_buff, "");
}
