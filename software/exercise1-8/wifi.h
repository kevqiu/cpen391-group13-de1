#define Wifi_Control (*(volatile unsigned char *)(0x84000240))
#define Wifi_Status (*(volatile unsigned char *)(0x84000240))
#define Wifi_TxData (*(volatile unsigned char *)(0x84000242))
#define Wifi_RxData (*(volatile unsigned char *)(0x84000242))
#define Wifi_Baud (*(volatile unsigned char *)(0x84000244))

void init_wifi(void);
void send_text(char* text);
void send_message(char* message);
char* receive_message(int length);
void print_message(char* message);
int put_char_wifi(int c);
int get_char_wifi(void);
int test_wifi_data(void);
