#define gps_Control (*(volatile unsigned char *)(0x84000210))
#define gps_Status (*(volatile unsigned char *)(0x84000210))
#define gps_TxData (*(volatile unsigned char *)(0x84000212))
#define gps_RxData (*(volatile unsigned char *)(0x84000212))
#define gps_Baud (*(volatile unsigned char *)(0x84000214))

#define GPGGA_COMMAND "$GPGGA"

#define TIMEZONE_DIFF 80000

void parse_gps_buffer(char* command, char* time_buffer);
void insert_time_colons(char* time);

int init_gps(void);
void send_command(char* message);
void print_message(char message[]);
int put_char_gps(char c);
char get_char_gps(void);
int is_gps_data_ready(void);
