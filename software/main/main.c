#include <stdio.h>
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

extern char Pepe[];

// Function prototypes for main loop
void poll_touchscreen(void);
void poll_timestamp(void);
void convert_epoch(char* timestamp, int time);

// GUI constants
extern rectangle buttons[];

// Touchscreen externs
extern point POINT;
extern int TS_STATE;

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
	int is_sweeping = 0;
	while(1)	
	{
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
		update_status();
	int is_changed = 0;
	if (prev_state != TS_STATE)
	{
		is_changed = 1;
		prev_state = TS_STATE;
	}
	if (is_changed == 1)
	{
		if (TS_STATE == TS_STATE_TOUCHED && !is_sweeping)
		{
			if (touch_in_button(POINT, (rectangle) SWEEP_CW_BTN))
			{
				is_sweeping = 1;
			}
		}

		if (touch_in_button(p, (rectangle) SWEEP_CW_BTN)) {
			curr_btn = 0;
			draw_rectangle((rectangle) SWEEP_CW_BTN, BOLDED);

			sweep(CW);
		}
			else if (touch_in_button(POINT, (rectangle) SWEEP_CCW_BTN)) {
				is_sweeping = 1;
			draw_rectangle((rectangle) SWEEP_CCW_BTN, BOLDED);

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
		} else {
		curr_btn = -1;

		else if (TS_STATE == TS_STATE_TOUCHED && is_sweeping)
		{
			printf("Sweeping...\n");
			return;
		}
		else 
			if (is_sweeping)
			{
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
	}


	// FOR DEBUGGING, REMOVE LATER
	leds = (TS_STATE == TS_STATE_UNTOUCHED);

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
