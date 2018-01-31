#include <stdio.h>
#include <string.h>

#include "wifi.h"
#include "arduino.h"
#include "rs232.h"
#include "graphics.h"
#include "touchscreen.h"
#include "io.h"

int main() {
	printf("Initializing...\n");

	init_touch();
	init_arduino();

	clear_screen();
	draw_screen();

	printf("Ready!\n");

	// DELETE THIS HARDCODED TIME STAMP AFTER
	char* timestamp = "18:11:30"; // replace this with GPS data
	WriteStringFont2(355, 240, WHITE, BLACK, timestamp);
	// DELETE THIS HARDCODED TIME STAMP AFTER

	int is_curr_pressed = 0, is_sweeping = 0;
	while(1) {
		// -------------------------------- //
		//       Servo Control Block        //
		// -------------------------------- //
		if (is_screen_touched() && !is_curr_pressed) {
			point p = get_press();
			if (touch_in_button(p, (rectangle) SWEEP_CW_BTN)) {
				is_curr_pressed = is_sweeping = 1;
				sweep(CW);
			}
			else if (touch_in_button(p, (rectangle) SWEEP_CCW_BTN)) {
				is_curr_pressed = is_sweeping = 1;
				sweep(CCW);
			}
			else if (touch_in_button(p, (rectangle) SET_180_BTN)) {
				is_curr_pressed = 1;
				set_servo(180);
			}
			else if (touch_in_button(p, (rectangle) SET_90_BTN)) {
				is_curr_pressed = 1;
				set_servo(90);
			}
			else if (touch_in_button(p, (rectangle) SET_0_BTN)) {
				is_curr_pressed = 1;
				set_servo(0);
			}
		}
		else {
			//printf("is screen touched: %i\n", is_screen_touched());
			//if (is_screen_touched()) get_press();
			is_curr_pressed = is_screen_touched();
			if (is_screen_touched()) get_press();
			// clear touch buffer
			//

			// stop sweeping if touch lock is off
			if(!is_curr_pressed && is_sweeping == 1) {
				is_sweeping = 0;
				sweep(STOP);
			}
		}

		// FOR DEBUGGING, REMOVE LATER
		leds = is_screen_touched();

		// -------------------------------- //
		//     Timestamp Display Block      //
		// -------------------------------- //
		/*
		 * get_timestamp_from_gps()
		 * draw_rectangle((rectangle) TIMESTAMP_BOX, FILLED);
		 * WriteStringFont2(355, 240, WHITE, BLACK, timestamp);
		 */

		// DELETE THIS IF BLOCK ONCE GPS IS GOOD, USE CODE ABOVE
		if (push_buttons & 0b001) {
			// if button pressed, redraw time
			draw_rectangle((rectangle) TIMESTAMP_BOX, FILLED);
			WriteStringFont2(355, 240, WHITE, BLACK, "23:59:59");
		}
		// DELETE THIS IF BLOCK ONCE GPS IS GOOD
	}

	printf("Complete!\n");
	return 0;
}
