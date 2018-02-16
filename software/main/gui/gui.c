#ifndef _GRAPHICS_
#include "graphics.h"
#include "gui.h"
#endif

/*
 * Boxes to be called externally in draw_screen.
 * Contains all rectangles (containers and buttons) to be displayed on GUI
 */
const rectangle boxes[] = {
	AUTO_SORT_BTN,
	STOP_BTN,
	SWEEP_CW_BTN,
	SWEEP_CCW_BTN,
	POS_1_BTN,
	POS_2_BTN,
	POS_3_BTN,
	POS_4_BTN,
	IMG_BOX,
	SCANNED_BOX_1,
	SCANNED_BOX_2,
	SCANNED_BOX_3,
	SCANNED_BOX_4
};

/*
 * Labels to be called externally in draw_screen.
 * Contains all labels (headers and buttons) to be displayed on GUI
 */
const text labels[] = {
	AUTOMATED_TXT,
	SORT_TXT,
	STOP_TXT,
	SWEEP_CW_TXT,
	SWEEP_CCW_TXT, 
	POS_1_TXT,
	POS_2_TXT,
	POS_3_TXT,
	POS_4_TXT,
	CONTROLS_TXT,
	SCANNED_IMG_TXT,
	TIME_SCAN_TXT,
	SCANNED_OBJ_TXT,
	RED_OBJ_TXT,
	GREEN_OBJ_TXT,
	BLUE_OBJ_TXT,
	OTHER_OBJ_TXT,
	CURR_TIME_TXT
};

/*
 * Counts of above arrays. To be called externally in draw_screen.
 */
int boxes_count = sizeof(boxes)/sizeof(boxes[0]);
int labels_count = sizeof(labels)/sizeof(labels[0]);