#include "fmain.h"

int main() {
	init_app();

	bool is_break = false;
	while(true) {
		refresh_app();
		int key;
 		key = getch();
		
		if(key == KEY_RESIZE) {
			resize_app();
		}
		else if (key == KEY_F(1) || key == '9') {
			is_break = true;
			close_app();
		}
		else if(key == KEY_MOUSE) {
			MEVENT mevent;
			if(getmouse(&mevent) == OK) {
				if(wenclose(wpanel->dep->fpanel->panel, mevent.y, mevent.x)) {
					wpanel = change_wpanel(wpanel);
					wmouse_handler(wpanel, mevent);
				} 
				else if(wenclose(upper_panel->subwin, mevent.y, mevent.x) && mevent.bstate & BUTTON1_RELEASED) {
					find_click_item(upper_panel, mevent);
					wrefresh(upper_panel->subwin);
					int choice;
					if(upper_panel->select == 0) {
						choice = show_setting_panel_window(0);
						change_mode(wpanels[0], choice);
					} 
					else if(upper_panel->select == 1) {
						choice = show_command_window();
						if(choice == 0) {
							wkeypad_handler(wpanel, 'S');
						} 
						else if(choice == 3) {
							wpanel->fpanel->is_sort = false;
							int select = wpanel->fpanel->file_menu->select;
							set_new_items(wpanel->fpanel->file_menu, load_dir(wpanel->fpanel), select);
						}
						else if(choice == 4) {
							find_file(wpanel->fpanel);
						}
					} 
					else if(upper_panel->select == 2) {
						choice = show_setting_panel_window(1);
						change_mode(wpanels[1], choice);
					}
				} else {
					wmouse_handler(wpanel, mevent);
				}
			}
		}
		else {
			wkeypad_handler(wpanel, key);
		} 

		if(is_break)
			break;
	}

	return 1;
} 