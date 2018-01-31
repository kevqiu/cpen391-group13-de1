#include <stdio.h>
#include <string.h>
#include "touchscreen.h"

// Debugging functions
void print_byte_as_bits(char val)
{
	int i = 7;
	for (; 0 <= i; i--)
	{
		printf("%c", (val & (1 << i)) ? '1' : '0');
	}
}

void print_bits(unsigned char *bytes, int num_bytes)
{
	printf("[ ");
	int i = 0;
	for (; i < num_bytes; i++)
	{
		print_byte_as_bits(bytes[i]);
		printf(" ");
	}
	printf("]\n");
}

/*****************************************************************************
** test if screen touched
*****************************************************************************/
int touch_in_button(point p, rectangle r) {
	return p.x > r.x1 && p.x < r.x2 && p.y > r.y1 && p.y < r.y2;
}

void update_status(void)
{
	if (is_screen_touched())
	{
		int c = get_char_touchscreen();
		if (c == TS_PRESS_EVENT) TS_STATE = TS_STATE_TOUCHED;
		else if (c == TS_RELEASE_EVENT) TS_STATE = TS_STATE_UNTOUCHED;
		else return;
		int response[4];
		int i = 0;
		for (; i < 4; i++)
		{
			response[i] = get_char_touchscreen();
		}
		int x = (((int)response[1]) << 7) + ((int)response[0]);
		int y = (((int)response[3]) << 7) + ((int)response[2]);
		POINT = get_calibrated_point(x, y);
	}

}

/**
 * Calibrates the raw coordinates from the touchscreen
 * Approximately returns the pixel touched on a 
 * 1024x600 resolution screen.
 * TODO: Double check with display to see if need better returned values
 * 			i.e. Percentage from origin, more resolution, etc.
 */
point get_calibrated_point(int raw_x, int raw_y)
{
	point p;
	p.x = TS_X_CALIBRATION * raw_x;
	p.y = TS_Y_CALIBRATION * raw_y;
	return p;
}

/**
 * Enable Touchscreen
 * Returns 1 if successful, 0 otherwise;
 */
int req_touch_enable()
{
	char request[4];
	request[0] = TS_SYNC;
	request[1] = TS_REQ_TOUCH_ENABLE_SIZE;
	request[2] = TS_REQ_TOUCH_ENABLE;
	request[3] = '\0';
	send_message(request);
	if (!validate_command())
	{
		printf("Touchscreen could not be enabled\n");
		return 0;
	}
	return 1;
}

/*****************************************************************************
** Validates Command
** - Obtains the SYNC bits
** - Make sure that it returns OK 
** - Currently only supports the basic functions:
** 		TOUCH_ENABLE, TOUCH_DISABLE
** TODO: Add support to validate all other commands
*****************************************************************************/
int validate_command(void)
{
	while (1)
	{
		int sync = get_char_touchscreen();
		if (sync == TS_SYNC)
			break;
	}
	char validation[3];
	int i = 0;
	for (; i < 3; i++)
	{
		validation[i] = get_char_touchscreen();
	}
	int s = validation[1];
	if (s == TS_RES_OK)
	{
		return 1;
	}
	else if (s == TS_RES_UNRECOGNIZED)
	{
		printf("Unrecognized Command\n");
		return 0;
	}
	else if (s == TS_RES_TIMEOUT)
	{
		printf("Timeout\n");
		return 0;
	}
	else
	{
		printf("Unknown/Unsupported Status\n");
		return 0;
	}
}

/*****************************************************************************
** Initialise touch screen controller
*****************************************************************************/
void init_touch(void)
{
	// Program 6850 and baud rate generator to communicate with touchscreen
	// Touchscreen operates at 9600 baud
	// 8 bits, no parity, 1 stop
	TS_Control = 0x15;
	TS_Baud = 0x05;

	if (!req_touch_enable())
	{
		printf("Touchscreen initialisation failed\n");
	}
}

/*****************************************************************************
** test if screen touched
*****************************************************************************/
int is_screen_touched(void)
{
	// return TRUE if any data received from 6850 connected to touchscreen
	// or FALSE otherwise
	if (!(0x01 & TS_Status))
		return 0;
	return 1;
}

/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void wait_for_touch()
{
	while (!is_screen_touched())
	{
	}
}

/**
 * Gets a touch event
 * TODO: Configure support for moving
 */
point get_touch_event(int event_type)
{
	int is_press = 0;
	while (!is_press)
	{
		wait_for_touch();
		int c = get_char_touchscreen();
		if (c == event_type)
		{
			is_press = 1;
		}
	}
	int response[4];
	int i = 0;
	for (; i < 4; i++)
	{
		response[i] = get_char_touchscreen();
	}
	int x = (((int)response[1]) << 7) + ((int)response[0]);
	int y = (((int)response[3]) << 7) + ((int)response[2]);
	return get_calibrated_point(x, y);
}
	

/*****************************************************************************
* This function waits for a touch screen press event and returns X,Y coord
*****************************************************************************/
point get_press(void)
{
	return get_touch_event(TS_PRESS_EVENT);
}

/*****************************************************************************
* This function waits for a touch screen release event and returns X,Y coord
*****************************************************************************/
point get_release(void)
{
	return get_touch_event(TS_RELEASE_EVENT);
}

// Stolen mostly from rs232
// Should make cleaner somehow
void send_message(char *message)
{
	int message_length = strlen(message);
	int i = 0;
	while (i < message_length)
	{
		put_char_touchscreen(message[i]);
		i++;
	}
}

int put_char_touchscreen(int c)
{
	// poll Tx bit in 6850 status register. Wait fsor it to become '1'
	while (!(0x02 & TS_Status))
	{
	}

	// Write the character to the 6850 TxData register.
	TS_TxData = c;

	return c;
}

int get_char_touchscreen(void)
{
	// poll Rx bit in 6850 status register. Wait for it to become '1'
	while (!(0x01 & TS_Status))
	{
	}

	// read received character from 6850 RxData register.
	return (int)TS_RxData;
}
