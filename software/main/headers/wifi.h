#define Wifi_Control (*(volatile unsigned char *)(0x84000240))
#define Wifi_Status (*(volatile unsigned char *)(0x84000240))
#define Wifi_TxData (*(volatile unsigned char *)(0x84000242))
#define Wifi_RxData (*(volatile unsigned char *)(0x84000242))
#define Wifi_Baud (*(volatile unsigned char *)(0x84000244))

void init_wifi(void);
void send_text(char* text);

void send_message_wifi(char* message);
char* receive_message_wifi(int length);
void print_message_wifi(char* message);

int get_char_wifi(void);
int put_char_wifi(int c);
int test_wifi_data(void);
