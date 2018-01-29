#define GraphicsCommandReg   		(*(volatile unsigned short int *)(0x84000000))
#define GraphicsStatusReg   		(*(volatile unsigned short int *)(0x84000000))
#define GraphicsX1Reg   			(*(volatile unsigned short int *)(0x84000002))
#define GraphicsY1Reg   			(*(volatile unsigned short int *)(0x84000004))
#define GraphicsX2Reg   			(*(volatile unsigned short int *)(0x84000006))
#define GraphicsY2Reg   			(*(volatile unsigned short int *)(0x84000008))
#define GraphicsColourReg   		(*(volatile unsigned short int *)(0x8400000E))
#define GraphicsBackGroundColourReg   	(*(volatile unsigned short int *)(0x84000010))

/************************************************************************************************
** This macro pauses until the graphics chip status register indicates that it is idle
***********************************************************************************************/

#define WAIT_FOR_GRAPHICS		while((GraphicsStatusReg & 0x0001) != 0x0001);

//Here are some software routines to drive some simple graphics functions

// #defined constants representing values we write to the graphics 'command' register to get
// it to draw something. You will add more values as you add hardware to the graphics chip
// Note DrawHLine, DrawVLine and DrawLine at the moment do nothing - you will modify these

#define DrawHLineCmd	0x1
#define DrawVLineCmd	0x2
#define DrawLineCmd		0x3
#define	PutAPixelCmd	0xA
#define	GetAPixelCmd	0xB
#define	ProgramPaletteColour    0x10

// defined constants representing colours pre-programmed into colour palette
// there are 256 colours but only 8 are shown below, we write these to the colour registers
//
// the header files "Colours.h" contains constants for all 256 colours
// while the course file ColourPaletteData.c contains the 24 bit RGB data
// that is pre-programmed into the palette

#define	BLACK			0
#define	WHITE			1
#define	RED				2
#define	LIME			3
#define	BLUE			4
#define	YELLOW			5
#define	CYAN			6
#define	MAGENTA			7

#define XRES 			800
#define YRES			480
#define TRUE			1

#define FONT_SPACING_1 	6
#define FONT_SPACING_2 	11


// Graphics library
void DrawScreen();
void ClearScreen();
void Rectangle(int x1, int x2, int y1, int y2, int colour);
void FilledRectangle(int x1, int x2, int y1, int y2, int colour);

// Font writers
void WriteStringFont1(int x, int y, int color, int bgColor, char* string);
void WriteStringFont2(int x, int y, int color, int bgColor, char* string);

// Helper functions
void WriteAPixel(int x, int y, int Colour);
int ReadAPixel(int x, int y);
void ProgramPalette(int PaletteNumber, int RGB);

// Hardware line commands
void DrawHLine(int x1, int x2, int y, int Colour);
void DrawVLine(int x, int y1, int y2, int Colour);
void DrawLine(int x1, int x2, int y1, int y2, int Colour);

// Software line commands
void HLine(int x1, int y1, int length, int Colour);
void VLine(int x1, int y1, int length, int Colour);
int abs(int a);
int sign(int a);
void Line(int x1, int y1, int x2, int y2, int Colour);
