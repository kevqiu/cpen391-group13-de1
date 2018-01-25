#include <stdio.h>
#include <string.h>

#include "wifi.h"

int main() {
	printf("Initializing...\n");

	init_wifi();

	send_text("Group 13 Exercise 1.8 Demo!");

	printf("Complete!\n");
	return 0;
}
