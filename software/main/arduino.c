#include <stdio.h>
#include <string.h>

#include "arduino.h"

void init_arduino(void)
{
	// set up 6850 Control Register to utilise a divide by 16 clock,
	// set RTS low, use 8 bits of data, no parity, 1 stop bit,
	// transmitter interrupt disabled
	Arduino_Control = 0x15;

	// program for 9600 baud
	Arduino_Baud = 0x05;
}

void set_servo(int pos) {
	char arduino_command[512];
	sprintf(arduino_command, "set:%d", pos);
	send_message_arduino(arduino_command);
}

void send_message_arduino(char* message) {
	int i = 0;
	while(message[i] != '\0') {
		put_char_arduino(message[i++]);
	}
	put_char_arduino('\r');
}

char* receive_message_arduino(int length) {
	char* buffer;
	int i = 0;
	while(i	< length) {
		char c = get_char_arduino();
		printf("Get: %c\n", c);
		if(c != '\0') {
			buffer[i] = c;
		}
		i++;
	}
	buffer[i] = '\0';
	return buffer;
}

void print_message_arduino(char* message) {
	int message_length = strlen(message);
	printf("Message: ");
	int i = 0;
	while(i < message_length) {
		printf("%c", message[i]);
		i++;
	}
	printf("\n");
}

int put_char_arduino(int c)
{
	// poll Tx bit in 6850 status register. Wait for it to become '1'
	while (!(0x02 & Arduino_Status)) {}

	// Write the character to the 6850 TxData register.
	Arduino_TxData = c;

	return c;
}

int get_char_arduino(void)
{
	// poll Rx bit in 6850 status register. Wait for it to become '1'
	while (!(0x01 & Arduino_Status)) {}

	// read received character from 6850 RxData register.
	return (int) Arduino_RxData;
}

// the following function polls the 6850 to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read
int test_arduino_data(void)
{
	return 0x01 & Arduino_Status;
	// Test Rx bit in 6850 serial comms chip status register
	// if RX bit is set, return TRUE, otherwise return FALSE
}
