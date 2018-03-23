/*
 * main.h contains enums for state machines present in the main loop
 * mode_state: state of the button presses
 * sort_state: state of the auto sort process
 */
typedef enum {
    MODE_IDLE,
    MODE_AUTO_SORT,
    MODE_SWEEP,
    MODE_OVERRIDE
} mode_state;

typedef enum {
    SORT_IDLE,
    SORT_INIT,
    SORT_INIT_WAIT,
    SORT_BEGIN_CYCLE,
    SORT_ARD_WAIT,     
    SORT_ARD_READY,
    SORT_CAM_READY,
    SORT_IMG_WAIT,
    SORT_IMG_READY
} sort_state;


// ---------- FUNCTION PROTOTYPES --------- //
void poll_gps(void);
void handle_gps(void);

void poll_touchscreen(void);
void handle_touchscreen(void);

void poll_arduino(void);
void handle_arduino(void);

void poll_rpi(void);
void handle_rpi(void);

void set_mode(mode_state state, int value);
void draw_silhouette(int size_x, int size_y);
void check_switches();

scanned_obj* init_scanned_obj(int colour, int position, point location);
