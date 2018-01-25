// Inputs
#define push_buttons (~(*(volatile unsigned char *)(0x0002060)) & 0b111)
#define switches (*(volatile char *) 0x0002000 & 0b11111111)

// Outputs
#define leds (*(char *) 0x0002010)
