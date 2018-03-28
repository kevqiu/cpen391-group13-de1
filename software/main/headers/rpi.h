#define RPi_Control (*(volatile unsigned char *)(0x84000200))
#define RPi_Status (*(volatile unsigned char *)(0x84000200))
#define RPi_TxData (*(volatile unsigned char *)(0x84000202))
#define RPi_RxData (*(volatile unsigned char *)(0x84000202))
#define RPi_Baud (*(volatile unsigned char *)(0x84000204))

void init_rpi(void);

void capture_image(char* gpgga_sentence);
void initialize_autosort(char* gpgga_sentence);

void send_message_rpi(char* message);

int get_char_rpi(void);
int put_char_rpi(int c);
int is_rpi_data_ready(void);
