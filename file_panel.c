#include "file_panel.h"
#include <stdlib.h>
#include <string.h>
#include "file_types.h"

bool init_file_panel(File_Panel** file_panel, int num) {
    *file_panel = (File_Panel*)calloc(1, sizeof(File_Panel));

    int height, width, starty, startx;
	FILE_PANEL_SIZE(height, width)
	PANEL_START_POS(starty, startx, num)
    (*file_panel)->panel = newwin(height, width, starty, startx);
    (*file_panel)->current_directory = (char*)calloc(strlen(START_DIR) + 1, sizeof(char));
    if ((*file_panel)->panel == NULL) {    // checking if the window has not been created
        return false;
    }
    keypad((*file_panel)->panel, TRUE);
    wattron((*file_panel)->panel, COLOR_PAIR(1));

    strcpy((*file_panel)->current_directory, START_DIR);

    ITEM** dir_file = load_dir(*file_panel);
    init_file_menu(*file_panel, dir_file);
    
    return true;
}

bool resize_file_panel(File_Panel* file_panel, int num)  {
    unpost_menu(file_panel->file_menu);
    int height, width, starty, startx;
    PANEL_START_POS(starty, startx, num)
    FILE_PANEL_SIZE(height, width)

    werase(file_panel->panel);
    wresize(file_panel->panel, height, width);
    mvwin(file_panel->panel, starty, startx);

    wresize(file_panel->menu_sub_win, height - FILE_MENU_FORMAT_SIZE_ROW, width - FILE_MENU_FORMAT_SIZE_COL);
    mvderwin(file_panel->menu_sub_win, FILE_MENU_START_ROW, FILE_MENU_START_COL);
    set_menu_format(file_panel->file_menu, height - FILE_MENU_FORMAT_ROW, FILE_MENU_FORMAT_COL);

    return true;
}

void refresh_file_panel(File_Panel* file_panel) {
    box(file_panel->panel, 0, 0);
    post_menu(file_panel->file_menu);
    wrefresh(file_panel->panel);
    refresh();
}

ITEM** load_dir(File_Panel* file_panel) {
    List* head = read_dir(START_DIR);
    int i = 0;

    ITEM **my_items;
    my_items = (ITEM **)calloc(sizeof_list(head) + 1, sizeof(ITEM *));

    List* temp = head;
    while(temp != NULL) {
        my_items[i] = new_item(temp->data, "");  
        temp = temp->next;
        i++;              
    }
    my_items[i] = NULL;      

    return my_items;
}

bool init_file_menu(File_Panel* file_panel, ITEM** items) {
    file_panel->file_menu = new_menu(items);
    int row, column;
    getmaxyx(file_panel->panel, row, column);

    file_panel->menu_sub_win = derwin(file_panel->panel, row - FILE_MENU_FORMAT_SIZE_ROW, column - FILE_MENU_FORMAT_COL, 
                                        FILE_MENU_START_ROW, FILE_MENU_START_COL);
    wattron(file_panel->menu_sub_win, COLOR_PAIR(1));
    set_menu_win(file_panel->file_menu, file_panel->panel);
    set_menu_sub(file_panel->file_menu, file_panel->menu_sub_win);
    set_menu_format(file_panel->file_menu, row - FILE_MENU_FORMAT_ROW, FILE_MENU_FORMAT_COL);

    post_menu(file_panel->file_menu);

    return true;
}