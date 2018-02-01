#include <stdio.h>
#include <string.h>
#include <time.h>

#include "wifi.h"
#include "arduino.h"
#include "rs232.h"
#include "graphics.h"
#include "touchscreen.h"
#include "gps.h"
#include "io.h"

extern char Pepe[];

// Function prototypes for main loop
void poll_touchscreen(void);
void poll_timestamp(void);
void convert_epoch(char* timestamp, int time);

// GUI constants
extern rectangle buttons[];

// State variables
int is_curr_pressed = 0,
	is_sweeping = 0,
	curr_btn = -1;
char curr_time[30] = "0000/00/0000:00:00";

int main() {
	printf("Initializing...\n");

	init_touch();
	init_arduino();

	clear_screen();

	//OutGraphics160x128(100,50, Pepe);
	draw_screen();

	printf("Ready!\n");

	// Main loop
	while(1) {
		poll_touchscreen();
		poll_timestamp();
	}

	printf("Complete!\n");
	return 0;
}

/*
 * Poll for touch screen inputs
 * Handles redrawing of GUI when buttons are pressed/depressed
 * Sends Arduino commands on key press
 */
void poll_touchscreen() {
	if (is_screen_touched() && !is_curr_pressed) {
		point p = get_press();
		if (touch_in_button(p, (rectangle) SWEEP_CW_BTN)) {
			curr_btn = 0;
			draw_rectangle((rectangle) SWEEP_CW_BTN, BOLDED);

			is_curr_pressed = is_sweeping = 1;
			sweep(CW);
		}
		else if (touch_in_button(p, (rectangle) SWEEP_CCW_BTN)) {
			curr_btn = 1;
			draw_rectangle((rectangle) SWEEP_CCW_BTN, BOLDED);

			is_curr_pressed = is_sweeping = 1;
			sweep(CCW);
		}
		else if (touch_in_button(p, (rectangle) SET_180_BTN)) {
			curr_btn = 2;
			draw_rectangle((rectangle) SET_180_BTN, BOLDED);

			is_curr_pressed = 1;
			set_servo(180);
		}
		else if (touch_in_button(p, (rectangle) SET_90_BTN)) {
			curr_btn = 3;
			draw_rectangle((rectangle) SET_90_BTN, BOLDED);

			is_curr_pressed = 1;
			set_servo(90);
		}
		else if (touch_in_button(p, (rectangle) SET_0_BTN)) {
			curr_btn = 4;
			draw_rectangle((rectangle) SET_0_BTN, BOLDED);

			is_curr_pressed = 1;
			set_servo(0);
		}
		else {
			curr_btn = -1;
		}
	}
	else {
		is_curr_pressed = is_screen_touched();
		// clear touch buffer
		if (is_screen_touched()) get_press();

		// stop sweeping if sweeping and not touching
		if(!is_curr_pressed && is_sweeping == 1) {
			is_sweeping = 0;
			sweep(STOP);
		}

		// redraw rectangle if not touching and touching a button
		if(!is_curr_pressed && curr_btn > -1) {
			clear_bolded_rectangle(buttons[curr_btn]);
			draw_rectangle(buttons[curr_btn], EMPTY);
			curr_btn = -1;
		}
	}

	// FOR DEBUGGING, REMOVE LATER
	leds = is_screen_touched();
}

/*
 * Fetches timestamp from GPS and updates the value on the screen
 * if the timestamp is different
 */
void poll_timestamp() {
	char new_time[30];
	int epoch_time = get_epoch_time();
	epoch_time += (switches << 8);
	convert_epoch(new_time, epoch_time);

	// if the timestamps are different, redraw the time
	if(strcmp(curr_time, new_time) != 0) {
		strcpy(curr_time, new_time);
		draw_rectangle((rectangle) TIMESTAMP_BOX, FILLED);
		WriteStringFont2(296, 240, WHITE, BLACK, curr_time);
	}
}

void convert_epoch(char* timestamp, int time) {
    time_t t = time;
    struct tm ts = *gmtime(&t);
    strftime(timestamp, 30*sizeof(char), "%Y/%m/%d %H:%M:%S", &ts);
}
