#include <string.h>

#include "graphics.h"

/*********************************************************************************************
* Graphics library
*********************************************************************************************/
void DrawScreen()
{
	// top left button
	Rectangle(50, 350, 40, 220, WHITE);
	WriteStringFont2(140, 125, LIME, BLACK, "Sweep Left");

	// top right button
	Rectangle(450, 750, 40, 220, WHITE);
	WriteStringFont2(540, 125, LIME, BLACK, "Sweep Right");

	// bottom left
	Rectangle(50, 250, 240, 420, WHITE);
	WriteStringFont2(90, 330, CYAN, BLACK, "180 degrees");

	// bottom middle
	Rectangle(300, 500, 240, 420, WHITE);
	WriteStringFont2(345, 330, CYAN, BLACK, "90 degrees");

	// bottom right
	Rectangle(550, 750, 240, 420, WHITE);
	WriteStringFont2(600, 330, CYAN, BLACK, "0 degrees");
}

void ClearScreen()
{
	int y = 0;
	while(y < YRES)
	{
		DrawHLine(0, XRES - 1, y++, BLACK);
	}
}

void Rectangle(int x1, int x2, int y1, int y2, int colour)
{
	DrawHLine(x1, x2, y1, colour);
	DrawHLine(x1, x2, y2, colour);
	DrawVLine(x1, y1, y2, colour);
	DrawVLine(x2, y1, y2, colour);
}

void FilledRectangle(int x1, int x2, int y1, int y2, int colour)
{
	int y = y1;
	while(y < y2) {
		DrawHLine(x1, x2, y++, colour);
	}
}

void WriteStringFont1(int x, int y, int color, int bgColor, char* string)
{
	int i;
	for(i = 0; i < strlen(string); i++)
	{
		OutGraphicsCharFont1(x+(i*FONT_SPACING_1), y, color, bgColor, string[i], 0);
	}
}

void WriteStringFont2(int x, int y, int color, int bgColor, char* string)
{
	int i;
	for(i = 0; i < strlen(string); i++)
	{
		OutGraphicsCharFont2(x+(i*FONT_SPACING_2), y, color, bgColor, string[i], 0);
	}
}

/*******************************************************************************************
* This function writes a single pixel to the x,y coords specified using the specified colour
* Note colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
********************************************************************************************/
void WriteAPixel(int x, int y, int Colour)
{
	WAIT_FOR_GRAPHICS;				// is graphics ready for new command

	GraphicsX1Reg = x;				// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour;			// set pixel colour
	GraphicsCommandReg = PutAPixelCmd;			// give graphics "write pixel" command
}

/*********************************************************************************************
* This function read a single pixel from the x,y coords specified and returns its colour
* Note returned colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
*********************************************************************************************/
int ReadAPixel(int x, int y)
{
	WAIT_FOR_GRAPHICS;			// is graphics ready for new command

	GraphicsX1Reg = x;			// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsCommandReg = GetAPixelCmd;		// give graphics a "get pixel" command

	WAIT_FOR_GRAPHICS;			// is graphics done reading pixel
	return (int)(GraphicsColourReg) ;	// return the palette number (colour)
}

/**********************************************************************************
** subroutine to program a hardware (graphics chip) palette number with an RGB value
** e.g. ProgramPalette(RED, 0x00FF0000) ;
**
************************************************************************************/
void ProgramPalette(int PaletteNumber, int RGB)
{
    WAIT_FOR_GRAPHICS;
    GraphicsColourReg = PaletteNumber;
    GraphicsX1Reg = RGB >> 16   ;        // program red value in ls.8 bit of X1 reg
    GraphicsY1Reg = RGB ;                // program green and blue into ls 16 bit of Y1 reg
    GraphicsCommandReg = ProgramPaletteColour; // issue command
}

/*********************************************************************************************
Hardware based line commands
*********************************************************************************************/
void DrawHLine(int x1, int x2, int y, int Colour)
{
	WAIT_FOR_GRAPHICS;					// is graphics ready for new command

	GraphicsX1Reg = x1;					// write coords to x1, x2, y1
	GraphicsX2Reg = x2;
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour;			// set pixel colour
	GraphicsCommandReg = DrawHLineCmd;	// give graphics "write pixel" command
}

void DrawVLine(int x, int y1, int y2, int Colour)
{
	WAIT_FOR_GRAPHICS;					// is graphics ready for new command

	GraphicsX1Reg = x;
	GraphicsY1Reg = y1;
	GraphicsY2Reg = y2;
	GraphicsColourReg = Colour;			// set pixel colour
	GraphicsCommandReg = DrawVLineCmd;		// give graphics "write pixel" command
}

void DrawLine(int x1, int x2, int y1, int y2, int Colour)
{
	WAIT_FOR_GRAPHICS;					// is graphics ready for new command

	GraphicsX1Reg = x1;
	GraphicsX2Reg = x2;
	GraphicsY1Reg = y1;
	GraphicsY2Reg = y2;
	GraphicsColourReg = Colour;			// set pixel colour
	GraphicsCommandReg = DrawLineCmd;	// give graphics "write pixel" command
}

/*********************************************************************************************
Sofware based line commands
*********************************************************************************************/
void HLine(int x1, int y1, int length, int Colour)
{
	int i;

	for(i = x1; i < x1+length; i++ )
		WriteAPixel(i, y1, Colour);
}

void VLine(int x1, int y1, int length, int Colour)
{
	int i;

	for(i = y1; i < y1+length; i++ )
		WriteAPixel(x1, i, Colour);
}

/*******************************************************************************
** Implementation of Bresenhams line drawing algorithm
*******************************************************************************/
int abs(int a)
{
    if(a < 0)
        return -a ;
    else
        return a ;
}

int sign(int a)
{
    if(a < 0)
        return -1 ;
    else if (a == 0)
        return 0 ;
    else
        return 1 ;
}


void Line(int x1, int y1, int x2, int y2, int Colour)
{
    int x = x1;
    int y = y1;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int s1 = sign(x2 - x1);
    int s2 = sign(y2 - y1);
    int i, temp, interchange = 0, error ;

    // if x1=x2 and y1=y2 then it is a line of zero length
    if(dx == 0 && dy == 0)
        return ;

	// must be a complex line so use bresenhams algorithm
    else {
    	// swap delta x and delta y depending upon slop of line
        if(dy > dx) {
            temp = dx ;
            dx = dy ;
            dy = temp ;
            interchange = 1 ;
        }

        // initialise the error term to compensate for non-zero intercept
        error = (dy << 1) - dx ;    // (2 * dy) - dx

		// main loop
        for(i = 1; i <= dx; i++)    {
            WriteAPixel(x, y, Colour);

            while(error >= 0)   {
                if(interchange == 1)
                    x += s1 ;
                else
                    y += s2 ;

                error -= (dx << 1) ;    // times 2
            }

            if(interchange == 1)
                y += s2 ;
            else
                x += s1 ;

            error += (dy << 1) ;    // times 2
        }
    }
}
