#define Camera_Control (*(volatile unsigned char *)(0x84000260))
#define Camera_Status (*(volatile unsigned char *)(0x84000260))
#define Camera_TxData (*(volatile unsigned char *)(0x84000262))
#define Camera_RxData (*(volatile unsigned char *)(0x84000262))
#define Camera_Baud (*(volatile unsigned char *)(0x84000264))

/*********************************************************************
 * A majority of the code is adapted from the Adafruit TTL JPEG Camera
 * repository
 * link: https://github.com/adafruit/Adafruit-VC0706-Serial-Camera-Library
 * ************************************************************************/

#define VC0706_SYNC 0x56
#define VC0706_RESET  0x26
#define VC0706_GEN_VERSION 0x11
#define VC0706_SET_PORT 0x24
#define VC0706_READ_FBUF 0x32
#define VC0706_GET_FBUF_LEN 0x34
#define VC0706_FBUF_CTRL 0x36
#define VC0706_DOWNSIZE_CTRL 0x54
#define VC0706_DOWNSIZE_STATUS 0x55
#define VC0706_READ_DATA 0x30
#define VC0706_WRITE_DATA 0x31
#define VC0706_COMM_MOTION_CTRL 0x37
#define VC0706_COMM_MOTION_STATUS 0x38
#define VC0706_COMM_MOTION_DETECTED 0x39
#define VC0706_MOTION_CTRL 0x42
#define VC0706_MOTION_STATUS 0x43
#define VC0706_TVOUT_CTRL 0x44
#define VC0706_OSD_ADD_CHAR 0x45

#define VC0706_STOPCURRENTFRAME 0x0
#define VC0706_STOPNEXTFRAME 0x1
#define VC0706_RESUMEFRAME 0x3
#define VC0706_STEPFRAME 0x2

#define VC0706_640x480 0x00
#define VC0706_320x240 0x11
#define VC0706_160x120 0x22

#define VC0706_MOTIONCONTROL 0x0
#define VC0706_UARTMOTION 0x01
#define VC0706_ACTIVATEMOTION 0x01

#define VC0706_SET_ZOOM 0x52
#define VC0706_GET_ZOOM 0x53

#define CAMERABUFFSIZ 100
#define CAMERADELAY 10

// If using more than one camera
#define SERIAL_NUM 0

// Global Variable Buffers to hold data from camera
int camera_buffer[CAMERABUFFSIZ+1];
int buffer_length;
long int frameptr;

//void print_byte_as_bits(char val);
//void print_bits(unsigned char *bytes, int num_bytes);

void init_camera(void);
void send_command_camera(int command, int args[], int argn);
int read_response(int num_bytes, int timeout);
int verify_response(int command);
int run_command(int command, int args[], int argn, int reslen);

void print_buffer(void);

int take_picture(void);
int camera_frame_buffer_ctrl(int command);

int get_char_camera(void);
int put_char_camera(int c);

int * read_picture(int n);

int get_image_size(void);
int set_image_size(int x);
