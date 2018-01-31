#include "structs.h"

#define TS_Control (*(volatile unsigned char *)(0x84000230))
#define TS_Status (*(volatile unsigned char *)(0x84000230))
#define TS_TxData (*(volatile unsigned char *)(0x84000232))
#define TS_RxData (*(volatile unsigned char *)(0x84000232))
#define TS_Baud (*(volatile unsigned char *)(0x84000234))

#define TS_SYNC 0x55

// REQUEST which can be sent to the touchscreen:
// More available from the microchip datasheet
#define TS_REQ_TOUCH_ENABLE 0x12
#define TS_REQ_TOUCH_ENABLE_SIZE 0x01
#define TS_REQ_TOUCH_DISABLE 0x13
#define TS_REQ_TOUCH_DISABLE_SIZE 0x01
#define TS_REQ_CALIBRATE 0x14
#define TS_REQ_CALIBRATE_SIZE 0x02


// RESPONSE received from touchscreen
// More available from the microchip datasheet
#define TS_RES_OK 0x00
#define TS_RES_UNRECOGNIZED 0x01
#define TS_RES_TIMEOUT 0x04

#define TS_X_CALIBRATION (800/4000.0)
#define TS_Y_CALIBRATION (480/4000.0)

#define TS_PRESS_EVENT 0x81
#define TS_RELEASE_EVENT 0x80

#define TS_STATE_TOUCHED 0x20
#define TS_STATE_UNTOUCHED 0x21

// Maintains the state
static int TS_STATE = TS_STATE_UNTOUCHED;
static point POINT;

// Function Definitions
int touch_in_button(point p, rectangle r);

void init_touch(void);
point get_press(void);
point get_release(void);
point get_touch_event(int event_type);
point get_calibrated_point(int raw_x, int raw_y);
int is_screen_touched(void);
void wait_for_touch();

// Use to access various states
void update_status(void);

void send_request(char* request, char* request_size);

char* receive_message(int length);
void send_message(char* message);

int req_touch_enable(void);

int validate_command(void);


// RS232 Interface
int put_char_touchscreen(int c);
int get_char_touchscreen(void);
