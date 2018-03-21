#define gps_Control (*(volatile unsigned char *)(0x84000210))
#define gps_Status (*(volatile unsigned char *)(0x84000210))
#define gps_TxData (*(volatile unsigned char *)(0x84000212))
#define gps_RxData (*(volatile unsigned char *)(0x84000212))
#define gps_Baud (*(volatile unsigned char *)(0x84000214))

#define GPGGA_COMMAND "$GPGGA"

#define LOCATION_UBC "UBC"

#define LOCATION_UOFT "UofT"
#define LAT_UOFT "4339.774,N"
#define LNG_UOFT "07923.742,W"

#define LOCATION_MCGILL "McGill"
#define LAT_MCGILL "4530.327,N"
#define LNG_MCGILL "07334.610,W"

#define TIMEZONE_DIFF 80000

void parse_gps_buffer(char* command, char* time_buffer);
void insert_time_colons(char* time);

void init_gps(void);
void send_command(char* message);
void print_message(char message[]);
int put_char_gps(char c);
char get_char_gps(void);
int is_gps_data_ready(void);
