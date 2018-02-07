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
	init_camera();
	int size = get_image_size();
	printf("Image size: %d\n", size);

	printf("Complete!\n");
	return 0;
}
/*
int main() {
	printf("Initializing...\n");

	init_touch();
	init_arduino();

	clear_screen();
	draw_screen();

	printf("Ready!\n");

	int is_curr_pressed = 0, is_sweeping = 0;
	while(1) {
		if (is_screen_touched() && !is_curr_pressed) {
			point p = get_press();
			if (touch_in_button(p, (button) SWEEP_CW_BTN)) {
				is_curr_pressed = is_sweeping = 1;
				sweep(CW);
			}
			else if (touch_in_button(p, (button) SWEEP_CCW_BTN)) {
				is_curr_pressed = is_sweeping = 1;
				sweep(CCW);
			}
			else if (touch_in_button(p, (button) SET_180_BTN)) {
				is_curr_pressed = 1;
				set_servo(180);
			}
			else if (touch_in_button(p, (button) SET_90_BTN)) {
				is_curr_pressed = 1;
				set_servo(90);
			}
			else if (touch_in_button(p, (button) SET_0_BTN)) {
				is_curr_pressed = 1;
				set_servo(0);
			}
		}
		else {
			//printf("is screen touched: %i\n", is_screen_touched());
			if (is_screen_touched()) get_press();
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
		leds = is_screen_touched();
		//printf("is curr pressed: %i\n", is_curr_pressed);

//		int btn_lock = 0, sweep_lock = 0;
//		else if(btn_lock == 0 && sweep_lock == 0) {
//			if(push_buttons & 0b001) {
//				btn_lock = 1;
//				set_servo(0);
//			}
//			else if(push_buttons & 0b010) {
//				btn_lock = sweep_lock = 1;
//				sweep(CCW);
//			}
//			else if(push_buttons & 0b100) {
//				btn_lock = sweep_lock = 1;
//				sweep(CW);
//			}
//		}
//
//		else {
//			// keep lock on as long as a button is pressed
//			btn_lock = is_screen_touched();
//
//			// stop sweeping if button lock is off
//			if(btn_lock == 0 && sweep_lock == 1) {
//				sweep_lock = 0;
//				sweep(STOP);
//			}
//		}
	}

	//	while(1) {
	//		if(push_buttons & 0b001) {
	//			ClearScreen();
	//		}
	//	}

	//	init_wifi();
	//
	//	send_text("Group 13 Exercise 1.8 Demo!");

	printf("Complete!\n");
	return 0;
}*/
