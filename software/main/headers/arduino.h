#define Arduino_Control (*(volatile unsigned char *)(0x84000250))
#define Arduino_Status (*(volatile unsigned char *)(0x84000250))
#define Arduino_TxData (*(volatile unsigned char *)(0x84000252))
#define Arduino_RxData (*(volatile unsigned char *)(0x84000252))
#define Arduino_Baud (*(volatile unsigned char *)(0x84000254))

typedef enum {
    CW,
    CCW,
    STOP
} sweep_state;

#define RED_POS     23
#define GREEN_POS   68
#define BLUE_POS    113
#define OTHER_POS   158

void init_arduino(void);

void set_conveyor(int state);
void auto_sort();
void sweep(sweep_state state);
void set_servo(int position);

void send_message_arduino(char* message);
char* receive_message_arduino(int length);
void print_message_arduino(char* message);

int get_char_arduino(void);
int put_char_arduino(int c);
int is_arduino_data_ready(void);
