#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gps.h"

/**************************************************************************
/* Subroutine to initialise the Wifi Port by writing some data
** to the internal registers.
** Call this function at the start of the program before you attempt
** to read or write to data via the Wifi port
**
** Refer to 6850 data sheet for details of registers and
***************************************************************************/
void init_gps(void)
{
	// set up 6850 Control Register to utilise a divide by 16 clock,
	// set RTS low, use 8 bits of data, no parity, 1 stop bit,
	// transmitter interrupt disabled
	gps_Control = 0x15;

	// program for 9600 baud
	gps_Baud = 0b111;
}

//sends $PMTK command to the gps
void send_command(char message[]) {
	int message_length = strlen(message);
	int i = 0;
	printf("Put: ");
	while(i < message_length) {
		printf("%c", message[i]);
		put_char_gps(message[i]);
		i++;
	}
	printf("\n");
}

void parse_gps_buffer(char* command, char* time_buffer) {
	// command: $GPGGA,xxxxxxxxxxx
	if(strstr(command,GPGGA_COMMAND) != NULL) {
		char *gpgga,*time;
		char *r = malloc(30);

		strcpy(r, command);
		gpgga = strsep(&r, ",");
		time = strsep(&r, ",");

		memcpy(time_buffer, time, 6);

		insert_time_colons(time_buffer);
	}
}

void insert_time_colons(char* time) {
	char hr[3], min[3], sec[3];
	hr[0] = time[0];
	hr[1] = time[1];
	hr[2] = '\0';
	min[0] = time[2];
	min[1] = time[3];
	min[2] = '\0';
	sec[0] = time[4];
	sec[1] = time[5];
	sec[2] = '\0';
	sprintf(time, "%s:%s:%s", hr, min, sec);
}

void receive_message(char buffer[]){
	int i = 0;
		char c = get_char_gps();
		buffer[i]=c;

		while(c!='\n') {

			c = get_char_gps();

				buffer[i] = c;

			i++;
		}
		printf(buffer);
}

void print_message(char* message) {
	printf("Message: ");
	int i = 0;
	while(1) {

		printf("%c", message[i]);
		if (message[i] == '\n') break;
		i++;
	}
}

int put_char_gps(char c){
	// poll Tx bit in 6850 status register. Wait for it to become '1'
	while (!(0x02 & gps_Status)) {}

	// Write the character to the 6850 TxData register.
	gps_TxData = c;

	return (int)c;
}

char get_char_gps(void)
{
	// poll Rx bit in 6850 status register. Wait for it to become '1'
	while (!(0x01 & gps_Status)) {}

	// read received character from 6850 RxData register.
	return (char) gps_RxData;
}

int is_gps_data_ready(void) {
	return 0x01 & gps_Status;
	// Test Rx bit in 6850 serial comms chip status register
	// if RX bit is set, return TRUE, otherwise return FALSE
}
