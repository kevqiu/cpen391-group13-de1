#include <string.h>

#ifndef _GRAPHICS_
#include "graphics.h"
#endif

#include "structs.h"

extern rectangle boxes[];
extern text labels[];
extern int boxes_count, labels_count;
extern unsigned int ColourPaletteData[];

/*********************************************************************************************
* Graphics library
*********************************************************************************************/
/*
 * Main GUI rendering function, draws buttons, boxes, and text
 */
void draw_screen()
{
	initialize_colour_space();

	draw_rectangle(AUTO_SORT_BTN, FILLED);
	draw_rectangle(STOP_BTN, FILLED);

	int i;
	// render all the boxes
	for(i = 0; i < boxes_count; i++) {
		rectangle rect = boxes[i];
		draw_rectangle(rect, EMPTY);
	}
	// render all the text
	for(i = 0; i < labels_count; i++) {
		text label = labels[i];
		draw_text(label);
	}

	reset_counters();

	draw_line((line) CENTER_LINE, VERTICAL);
	draw_line((line) TOP_RIGHT_LINE, HORIZONTAL);
	draw_line((line) BTM_RIGHT_LINE, HORIZONTAL);
}

/*
 * Helper function to draw rectangles
 * Input: 
 * 	rectangle: struct of x1, x2, y1, y2, colour
 * 	fill: possible values - EMPTY, FILLED, BOLDED
 */
void draw_rectangle(rectangle rect, rect_fill fill)
{
	if (fill == EMPTY) {
		Rectangle(rect.x1, rect.x2, rect.y1, rect.y2, rect.colour);
	}
	else if (fill == FILLED) {
		FilledRectangle(rect.x1, rect.x2, rect.y1, rect.y2, rect.colour);
	}
	else if (fill == BOLDED) {
		BoldedRectangle(rect.x1, rect.x2, rect.y1, rect.y2, BOLDED_RECT_THICKNESS, BLACK);
	}
}

/*
 * Helper function to draw rectangles
 * Input: 
 * 	text: struct of x, y coordinates, colour, and text
 */
void draw_text(text txt)
{
	WriteStringFont2(txt.x, txt.y, txt.colour, BLACK, txt.text, 0);
}

/*
 * Helper function to hardware accelerated lines
 * Input: 
 * 	line: struct of x, y coordinates, length, colour
 * 	direction: possible values - HORIZONTAL, VERTICAL
 */
void draw_line(line line, line_dir direction) 
{
	if (direction == HORIZONTAL) {
		DrawHLine(line.x, line.x + line.len, line.y, line.colour);
	}
	else if (direction == VERTICAL) {
		DrawVLine(line.x, line.y, line.y + line.len, line.colour);
	}
}

/*
 * Helper function to write object count inside counter box
 * Input: 
 * 	point: x, y coordinate of count text
 *  count: number of objects counted
 */
void draw_counter(point p, int count) {
	char text[3];
	if (count < 10) {
		sprintf(text, "0%i", count);
	}
	else {
		sprintf(text, "%i", count);
	}
	WriteStringFont2(p.x, p.y, BLACK, WHITE, text, 1);
}

/*
 * Helper function to reset object counts inside counter boxes to 0
 */
void reset_counters() {
	draw_counter(RED_OBJ_LOC, 0);
	draw_counter(GREEN_OBJ_LOC, 0);
	draw_counter(BLUE_OBJ_LOC, 0);
	draw_counter(OTHER_OBJ_LOC, 0);
}

/*
 * Helper function to reset button to default non-bolded state
 */
void reset_button(rectangle rect) {
	clear_bolded_rectangle(rect);
	draw_rectangle(rect, EMPTY);
}

/*
 * Helper function to wipe screen with background colour
 */
void clear_screen()
{
	int y = 0;
	while(y < YRES)
	{
		DrawHLine(0, XRES - 1, y++, BG_COLOUR);
	}
}

/*
 * Helper function to remove bolding from a rectangle
 */
void clear_bolded_rectangle(rectangle rect)
{
	BoldedRectangle(rect.x1, rect.x2, rect.y1, rect.y2, BOLDED_RECT_THICKNESS, BG_COLOUR);
}

/*
 * Helper function to initialize first 64 colours in the provided palette
 */
void initialize_colour_space() {
	int i;
	for(i = 0; i < 64; i++) {
		ProgramPalette(i, ColourPaletteData[i]);
	}
}

/*
 * Function to draw a rectangle
 */
void Rectangle(int x1, int x2, int y1, int y2, int colour)
{
	DrawHLine(x1, x2, y1, colour);
	DrawHLine(x1, x2, y2, colour);
	DrawVLine(x1, y1, y2, colour);
	DrawVLine(x2, y1, y2, colour);
}

/*
 * Function to draw a filled rectangle
 */
void FilledRectangle(int x1, int x2, int y1, int y2, int colour)
{
	int y = y1;
	while(y < y2) {
		DrawHLine(x1, x2, y++, colour);
	}
}

/*
 * Function to draw a rectangle wtih bolded edges
 */
void BoldedRectangle(int x1, int x2, int y1, int y2, int thickness, int colour)
{
	int i;
	for(i = 0; i < thickness; i++)
	{
		Rectangle(x1 - i, x2 + i, y1 - i, y2 + i, colour);
	}
}

/*
 * Function to draw the smaller text
 */
void WriteStringFont1(int x, int y, int color, int bgColor, char* string, int erase)
{
	int i;
	for(i = 0; i < strlen(string); i++)
	{
		OutGraphicsCharFont1(x+(i*FONT_SPACING_1), y, color, bgColor, string[i], 0);
	}
}

/*
 * Function to draw the bigger text
 */
void WriteStringFont2(int x, int y, int color, int bgColor, char* string, int erase)
{
	int i;
	for(i = 0; i < strlen(string); i++)
	{
		OutGraphicsCharFont2(x+(i*FONT_SPACING_2), y, color, bgColor, string[i], erase);
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
