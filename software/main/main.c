#include <stdio.h>
#include <string.h>

#include "wifi.h"
#include "arduino.h"
#include "rs232.h"
#include "io.h"

int main() {
	printf("Initializing...\n");

	//init_wifi();

	//send_text("Group 13 Exercise 1.8 Demo!");

	init_arduino();

	int btn_lock = 0;
	while(1) {
		if(btn_lock == 0) {
			if(push_buttons & 0b001) {
				btn_lock = 1;
				set_servo(0);
			}
			else if(push_buttons & 0b010) {
				btn_lock = 1;
				set_servo(90);
			}
			else if(push_buttons & 0b100) {
				btn_lock = 1;
				set_servo(180);
			}
		}
		else {
			btn_lock = push_buttons > 0;
		}

		leds = push_buttons | (btn_lock << 4);
	}

	printf("Complete!\n");
	return 0;
}
