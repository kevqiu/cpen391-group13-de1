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

typedef enum {
    ON,
    OFF
} conv_state;

#define RED_POS     180
#define GREEN_POS   120
#define BLUE_POS    60
#define OTHER_POS   0

void init_arduino(void);

void set_conveyor(int state);
void auto_sort();
void sweep(sweep_state state);
void conveyor(conv_state state);
void stop();
void set_servo(int position);

void send_message_arduino(char* message);
char* receive_message_arduino(int length);
void print_message_arduino(char* message);

int get_char_arduino(void);
int put_char_arduino(int c);
int is_arduino_data_ready(void);
