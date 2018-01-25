#define RS232_Control (*(volatile unsigned char *)(0x84000200))
#define RS232_Status (*(volatile unsigned char *)(0x84000200))
#define RS232_TxData (*(volatile unsigned char *)(0x84000202))
#define RS232_RxData (*(volatile unsigned char *)(0x84000202))
#define RS232_Baud (*(volatile unsigned char *)(0x84000204))

void init_rs232(void);

void send_message_rs232(char* message);
char* receive_message_rs232(int length);
void print_message_rs232(char* message);

int get_char_rs232(void);
int put_char_rs232(int c);
int test_rs323_data(void);
