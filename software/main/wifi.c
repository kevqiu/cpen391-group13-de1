#include <stdio.h>
#include <string.h>

#include "wifi.h"

void init_wifi(void)
{
	// set up 6850 Control Register to utilise a divide by 16 clock,
	// set RTS low, use 8 bits of data, no parity, 1 stop bit,
	// transmitter interrupt disabled
	Wifi_Control = 0x15;

	// program for 115k baud
	Wifi_Baud = 0x01;

	// initialize lua script
	send_message_wifi("dofile(\"send_text_message.lua\")");
}

// 604-670-3470 is the Twilio number
// this sends it to Kevin's number, pls don't spam me :(
void send_text(char* text) {
	char text_message[512];
	sprintf(text_message, "send_sms(\"(604) 670-3470\", \"(604) 779-2689\", \"%s\")", text);
	send_message_wifi(text_message);
}

void send_message_wifi(char* message) {
	int i = 0;
	while(message[i] != '\0') {
		put_char_wifi(message[i++]);
	}
	put_char_wifi('\n');
}

char* receive_message_wifi(int length) {
	char* buffer;
	int i = 0;
	while(i	< length) {
		char c = get_char_wifi();
		printf("Get: %c\n", c);
		if(c != '\0') {
			buffer[i] = c;
		}
		i++;
	}
	buffer[i] = '\0';
	return buffer;
}

void print_message_wifi(char* message) {
	printf("Message: ");
	int i = 0;
	while(message[i] != '\0') {
		printf("%c", message[i++]);
	}
	printf("\n");
}

int put_char_wifi(int c)
{
	// poll Tx bit in 6850 status register. Wait for it to become '1'
	while (!(0x02 & Wifi_Status)) {}

	// Write the character to the 6850 TxData register.
	Wifi_TxData = c;

	return c;
}

int get_char_wifi(void)
{
	// poll Rx bit in 6850 status register. Wait for it to become '1'
	while (!(0x01 & Wifi_Status)) {}

	// read received character from 6850 RxData register.
	return (int) Wifi_RxData;
}

// the following function polls the 6850 to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read
int test_wifi_data(void)
{
	return 0x01 & Wifi_Status;
	// Test Rx bit in 6850 serial comms chip status register
	// if RX bit is set, return TRUE, otherwise return FALSE
}
