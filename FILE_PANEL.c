#include "FILE_PANEL.h"
#include <stdlib.h>
#include <string.h>
#include "FILE_TYPES.h"

bool init_file_panel(FILE_PANEL** file_panel, int num) {
    *file_panel = (FILE_PANEL*)calloc(1, sizeof(FILE_PANEL));

    int height, width, starty, startx;
	FILE_PANEL_SIZE(height, width)
	PANEL_START_POS(starty, startx, num)
    (*file_panel)->panel = newwin(height, width, starty, startx);
    (*file_panel)->current_directory = (char*)calloc(strlen(START_DIR) + 1, sizeof(char));
    strcpy((*file_panel)->current_directory, START_DIR);

    printw((*file_panel)->current_directory);
    if ((*file_panel)->panel == NULL) {    // checking if the window has not been created
        return false;
    }
    keypad((*file_panel)->panel, TRUE);
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
    keypad(file_panel->panel, TRUE);
    wattron(file_panel->panel, COLOR_PAIR(1));
    return true;
}

void show_file_panel(FILE_PANEL* file_panel) {
    box(file_panel->panel, 0, 0);
    wbkgd(file_panel->panel, COLOR_PAIR(1));
    wrefresh(file_panel->panel);
}

void load_start_dir(FILE_PANEL* file_panel) {
    List* head = read_dir(START_DIR);
    int i = 0;

    ITEM **my_items;
    my_items = (ITEM **)calloc(sizeof_list(head), sizeof(ITEM *));

    List* temp = head;
    while(temp != NULL) {
        my_items[i] = new_item(temp->data, "");  
        temp = temp->next;
        i++;              
    }      
    file_panel->file_menu = new_menu((ITEM **)my_items);

    int row, column;
    getmaxyx(file_panel->panel, row, column);
    WINDOW* sub_win = derwin(file_panel->panel, row - 3, column - 1, 3, 1);
    set_menu_win(file_panel->file_menu, file_panel->panel);
    set_menu_sub(file_panel->file_menu, sub_win);
    set_menu_format(file_panel->file_menu, row - 3, 1);

    post_menu(file_panel->file_menu);
    wrefresh(file_panel->panel);

}

