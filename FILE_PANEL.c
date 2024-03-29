#include "FILE_PANEL.h"
#include <stdlib.h>
#include <string.h>

bool init_file_panel(FILE_PANEL** file_panel, int num) {
    *file_panel = (FILE_PANEL*)calloc(1, sizeof(FILE_PANEL));

    int height, width, starty, startx;
	FILE_PANEL_SIZE(height, width)
	PANEL_START_POS(starty, startx, num)
    (*file_panel)->panel = newwin(height, width, starty, startx);
    printw("%d %d", starty, startx);
    (*file_panel)->current_directory = (char*)calloc(strlen(START_DIR) + 1, sizeof(char));
    strncpy((*file_panel)->current_directory, START_DIR, strlen(START_DIR) + 1);

    if ((*file_panel)->panel == NULL) {    // checking if the window has not been created
        return false;
    }
    wattron((*file_panel)->panel, COLOR_PAIR(1));
    return true;
}

bool resize_file_panel(FILE_PANEL* file_panel, int num)  {
    delwin(file_panel->panel);

    int height, width, starty, startx;
	FILE_PANEL_SIZE(height, width)
	PANEL_START_POS(starty, startx, num)
    file_panel->panel = newwin(height, width, starty, startx);

    if (file_panel->panel == NULL) {    // checking if the window has not been created
        return false;
    }

    wattron(file_panel->panel, COLOR_PAIR(1));
    return true;
}

void show_file_panel(FILE_PANEL* file_panel) {
    box(file_panel->panel, 0, 0);
    wbkgd(file_panel->panel, COLOR_PAIR(1));
    wrefresh(file_panel->panel);
}