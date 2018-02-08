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

	get_char_arduino();
}

void auto_sort() {
	char* arduino_command = "as:1";
	send_message_arduino(arduino_command);
}

// 0 = CW, 1 = CCW
void sweep(sweep_state state) {
	char arduino_command[6];

	if(state == CW) {
		sprintf(arduino_command, "cw:1");
	}
	else if(state == CCW) {
		sprintf(arduino_command, "ccw:1");
	}
	else {
		sprintf(arduino_command, "");
	}
	send_message_arduino(arduino_command);
}

void conveyor(conv_state state) {
	char arduino_command[6];

	if(state == ON) {
		sprintf(arduino_command, "cv:1");
	}
	else if(state == OFF) {
		sprintf(arduino_command, "cv:0");
	}
	send_message_arduino(arduino_command);
}

void stop() {
	char* arduino_command = "stop";
	send_message_arduino(arduino_command);
}

void set_servo(int pos) {
	char arduino_command[6];
	sprintf(arduino_command, "s:%d", pos);
	send_message_arduino(arduino_command);
}

void send_message_arduino(char* message) {
	//printf("sending command: %s\n", message);
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
int is_arduino_data_ready(void)
{
	return 0x01 & Arduino_Status;
	// Test Rx bit in 6850 serial comms chip status register
	// if RX bit is set, return TRUE, otherwise return FALSE
}
