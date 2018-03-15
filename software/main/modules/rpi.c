#include <stdio.h>
#include <string.h>

#include "rpi.h"

void init_rpi(void) {
	// set up 6850 Control Register to utilise a divide by 16 clock,
	// set RTS low, use 8 bits of data, no parity, 1 stop bit,
	// transmitter interrupt disabled
	RPi_Control = 0x15;

	// program for 115k baud
	RPi_Baud = 0x05;

	get_char_rpi();
}

void capture_image(char* gpgga_sentence) {
	char message[256];
	sprintf(message, "capture: gps=%s", gpgga_sentence);
	send_message_rpi(message);
}

/*
 * Sends serial message to RPi
 * Terminates in carriage return to tell RPi command has been fully sent
 */
void send_message_rpi(char* message) {
	int i = 0;
	while(message[i] != '\0') {
		put_char_rpi(message[i++]);
	}
	put_char_rpi('\r');
}

void print_message_rpi(char* message) {
	int message_length = strlen(message);
	printf("Message: ");
	int i = 0;
	while(i < message_length) {
		printf("%c", message[i]);
		i++;
	}
	printf("\n");
}

int put_char_rpi(int c) {
	// poll Tx bit in 6850 status register. Wait for it to become '1'
	while (!(0x02 & RPi_Status)) {}

	// Write the character to the 6850 TxData register.
	RPi_TxData = c;

	return c;
}

int get_char_rpi(void) {
	// poll Rx bit in 6850 status register. Wait for it to become '1'
	while (!(0x01 & RPi_Status)) {}

	// read received character from 6850 RxData register.
	return (int) RPi_RxData;
}

// the following function polls the 6850 to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read
int is_rpi_data_ready(void) {
	return 0x01 & RPi_Status;
	// Test Rx bit in 6850 serial comms chip status register
	// if RX bit is set, return TRUE, otherwise return FALSE
}
