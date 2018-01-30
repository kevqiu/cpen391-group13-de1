#define Arduino_Control (*(volatile unsigned char *)(0x84000250))
#define Arduino_Status (*(volatile unsigned char *)(0x84000250))
#define Arduino_TxData (*(volatile unsigned char *)(0x84000252))
#define Arduino_RxData (*(volatile unsigned char *)(0x84000252))
#define Arduino_Baud (*(volatile unsigned char *)(0x84000254))

#define CW 0
#define CCW 1
#define STOP 2

void init_arduino(void);

void sweep(int direction);
void set_servo(int position);

void send_message_arduino(char* message);
char* receive_message_arduino(int length);
void print_message_arduino(char* message);

int get_char_arduino(void);
int put_char_arduino(int c);
int test_arduino_data(void);
