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

extern char Trump[];

// Function prototypes for main loop
void poll_gps(void);
void handle_gps(void);

void poll_touchscreen(void);
void handle_touchscreen(void);

// GUI variables
extern rectangle buttons[];
char curr_time[12] = "00:00:00";

// Touchscreen externs
extern point POINT;
extern int TS_STATE;

// State variables
int is_sweeping = 0,
	prev_state = TS_STATE_UNTOUCHED,
	curr_btn = -1;

// Serial handling
char gps_buffer[256],
	touchscreen_buffer[256];

int gps_inc = 0,
	touchscreen_inc = 0;

int gps_ready = 0,
	touchscreen_ready = 0;

int main() {
	printf("Initializing...\n");

	init_touch();
	init_gps();
	init_arduino();

	clear_screen();

	OutGraphics160x128(320, 60, Trump);
	draw_screen();

	printf("Ready!\n");
	
	// Main loop
	while(1)	
	{
		poll_touchscreen();
		poll_gps();

		if(touchscreen_ready) {
			handle_touchscreen();
			touchscreen_ready = 0;
		}

		//handle_touchscreen();

		if(gps_ready) {
			handle_gps();
			gps_inc = gps_ready = 0;
		}
	}

	printf("Complete!\n");
	return 0;
}

// Poll for GPS data
void poll_gps() {
	if(is_gps_data_ready()) {
		char c = get_char_gps();
		gps_buffer[gps_inc++] = c;
		if(c == '\n') {
			gps_ready = 1;
		}
	}
}

// Poll for Touchscreen data
void poll_touchscreen() {
	if (is_screen_touched())
	{
		int c = get_char_touchscreen();
		if (c == TS_PRESS_EVENT) {
			TS_STATE = TS_STATE_TOUCHED;
		}
		else if (c == TS_RELEASE_EVENT)
		{
			TS_STATE = TS_STATE_UNTOUCHED;
		}
		else return;
		int response[4];
		int i = 0;
		for (; i < 4; i++)
		{
			response[i] = get_char_touchscreen();
		}
		int x = (((int)response[1]) << 7) + ((int)response[0]);
		int y = (((int)response[3]) << 7) + ((int)response[2]);
		POINT = get_calibrated_point(x, y);
		touchscreen_ready = 1;
	}
}
	
/*
 * Fetches timestamp from GPS and updates the value on the screen
 * if the timestamp is different
 */
void handle_gps() {
	char new_time[12] = "";
	parse_gps_buffer(gps_buffer, new_time);

	// if the timestamps are different, redraw the time
	if(new_time[0] != '\0' && strcmp(curr_time, new_time) != 0) {
		// save new time
		strcpy(curr_time, new_time);

		// redraw rectangle
		draw_rectangle((rectangle) TIMESTAMP_BOX, FILLED);
		WriteStringFont2(365, 240, WHITE, BLACK, curr_time);
	}
}

void handle_touchscreen() {
	int is_changed = 0;
	if (prev_state != TS_STATE) {
		is_changed = 1;
		prev_state = TS_STATE;
	}
	if (is_changed == 1) {
		if (TS_STATE == TS_STATE_TOUCHED && !is_sweeping) {
			if (touch_in_button(POINT, (rectangle) SWEEP_CW_BTN)) {
				curr_btn = 0;
				draw_rectangle((rectangle) SWEEP_CW_BTN, BOLDED);

				is_sweeping = 1;
				sweep(CW);
			}
			else if (touch_in_button(POINT, (rectangle) SWEEP_CCW_BTN)) {
				curr_btn = 1;
				draw_rectangle((rectangle) SWEEP_CCW_BTN, BOLDED);

				is_sweeping = 1;
				sweep(CCW);
			}
			else if (touch_in_button(POINT, (rectangle) SET_180_BTN)) {
				curr_btn = 2;
				draw_rectangle((rectangle) SET_180_BTN, BOLDED);

				set_servo(180);
			}
			else if (touch_in_button(POINT, (rectangle) SET_90_BTN)) {
				curr_btn = 3;
				draw_rectangle((rectangle) SET_90_BTN, BOLDED);

				set_servo(90);
			}
			else if (touch_in_button(POINT, (rectangle) SET_0_BTN)) {
				curr_btn = 4;
				draw_rectangle((rectangle) SET_0_BTN, BOLDED);

				set_servo(0);
			}
			else {
				curr_btn = -1;
			}
		}
		else {
			if (is_sweeping) {
				is_sweeping = 0;
				sweep(STOP);
			}
			
			// redraw rectangle if not touching and touching a button
			if (curr_btn > -1) {
				clear_bolded_rectangle(buttons[curr_btn]);
				draw_rectangle(buttons[curr_btn], EMPTY);
				curr_btn = -1;
			}
		}
	}
}
