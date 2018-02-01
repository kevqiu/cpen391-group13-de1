#include <stdio.h>
#include <string.h>

#include "../headers/camera.h"

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

void init_camera(void)
{
    // Set 6850 Control register to  
    // 8 data, no parity, 1 stop
    Camera_Control = 0x14;

    // Baud rate set to 38400
    // !! DO NOT CHANGE !!
    Camera_Baud = 0x03;

    frameptr = 0;
}


void send_command_camera(int command, int args[], int argn)
{
    put_char_camera(VC0706_SYNC);
    put_char_camera(SERIAL_NUM);
    put_char_camera(command);
    int i = 0;
    for (; i < argn; i++)
    {
        put_char_camera(args[i]);
    }
}

int read_response(int num_bytes, int timeout)
{
    // TODO: See if a timout counter is needed;
    int buffer_index = 0;
    while (buffer_index != num_bytes)
    {
        camera_buffer[buffer_index] - get_char_camera();
        buffer_index++;
    }
    return buffer_index;
}

int verify_response(int command)
{
      if ((camera_buffer[0] != 0x76) ||
      (camera_buffer[1] != SERIAL_NUM) ||
      (camera_buffer[2] != command) ||
      (camera_buffer[3] != 0x0))
      return false;
  return true;
}

void print_buffer(void)
{
    print_bits(camera_buffer, buffer_length);
}

int get_char_camera(void)
{
    while (!(0x01 & Camera_Status)) {}
    return (int) Camera_RxData;
}

int put_char_camera(int c)
{
    while (!(0x02 & Camera_Status)) {}
    Camera_TxData = c;
    return c;
}

// MOD: removed the flush command
int run_command(int command, int args[], int argn, int reslen)
{
    send_command_camera(command, args, argn);
    if (read_response(reslen, 0) != reslen) return 0;
    if (!verify_response(command)) return 0;
    return 1;
}

int take_picture(void)
{
    frameptr = 0;
    return camera_frame_buff_ctrl(VC0706_STOPCURRENTFRAME);
}

int camera_frame_buff_ctrl(int command)
{
    int args[] = {SERIAL_NUM, command};
    //TODO: Is the 5 right here? Does this need to be the control flag
    return run_command(VC0706_FBUF_CTRL, args, sizeof(args), 5);
}

