// Inputs
#define push_buttons (~(*(volatile unsigned char *)(0x0002060)) & 0b111)
#define switches (*(volatile char *) 0x0002000 & 0b11111111)

// Outputs
#define leds (*(char *) 0x0002010)

#define HEX_0 (*(char *) 0x0002030)
#define HEX_1 (*(char *) 0x0002040)
#define HEX_2 (*(char *) 0x0002050)
#define HEX_3 (*(char *) 0x00020d0)
#define HEX_4 (*(char *) 0x00020e0)
#define HEX_5 (*(char *) 0x00020f0)

void display_hex(char* string);
void set_display(int seg_bits, int display);
