#include <stdio.h>
#include <string.h>

#include "wifi.h"
#include "arduino.h"
#include "rs232.h"
#include "graphics.h"
#include "io.h"

int main() {
	printf("Initializing...\n");

	DrawScreen();

	init_arduino();

	int btn_lock = 0, sweep_lock = 0;
	while(1) {
		if(btn_lock == 0 && sweep_lock == 0) {
			if(push_buttons & 0b001) {
				btn_lock = 1;
				set_servo(0);
			}
			else if(push_buttons & 0b010) {
				btn_lock = sweep_lock = 1;
				sweep(CCW);
			}
			else if(push_buttons & 0b100) {
				btn_lock = sweep_lock = 1;
				sweep(CW);
			}
		}
		else {
			// keep lock on as long as a button is pressed
			btn_lock = push_buttons > 0;

			// stop sweeping if button lock is off
			if(btn_lock == 0 && sweep_lock == 1) {
				sweep_lock = 0;
				sweep(STOP);
			}
		}

		if(switches & 0b1) {
			ClearScreen();
		}
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
}
