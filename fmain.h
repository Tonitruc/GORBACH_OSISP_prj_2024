#include <panel.h>
#include "file_panel.h"
#include <signal.h>
#include <locale.h>
#include <stdbool.h>
#include <regex.h>
#include "file_types.h"
#include <stdlib.h>
#include <form.h>
#include "menu.h"
#include "text_box.h"
#include "message_box.h"
#include "wpanel.h"

#define MIN_HEIGHT 15
#define MIN_WIDTH 15

#define AMOUNT_OF_FILE_PANEL 2

//File panels
extern WPANEL* wpanels[2];
extern WPANEL* wpanel;

//Menus for manage files
#define AMOUNT_TOP_PANEL_ITEMS 3
extern MENU* upper_panel;
extern WINDOW* setting_panel_window;

extern MENU* setting_panel_menu;

#define AMOUNT_SETTINGS 3

extern const wchar_t* setting_points[];

extern WINDOW* command_panel_window;
extern MENU* command_panel_menu;

#define AMOUNT_COMMAND 5

extern const wchar_t* command_points[];

extern const wchar_t* upper_panel_points[];
#define AMOUNT_LOWER_PANEL_ITEMS 9
extern MENU* lower_panel;

extern const wchar_t* lower_panel_points[];

//Levels of windows 
extern PANEL* application[2];

#define INIT_BASE_WIN init_pair(1, COLOR_WHITE, COLOR_DEEP_BLUE); attron(COLOR_PAIR(1)); \
					wbkgd(stdscr, COLOR_PAIR(1)); refresh();  //Set the base style for the main window
#define TOP_PANEL_SIZE(height, width) height = 1; width = getmaxx(stdscr)

bool init_app();
void init_lower_panel();
void init_upper_panel();
void init_command_menu();
void init_setting_menu();

void refresh_app();
void close_app();
bool resize_app();
int show_command_window();
int show_setting_panel_window(int panel_num);
void find_file(FILE_PANEL* file_panel);
