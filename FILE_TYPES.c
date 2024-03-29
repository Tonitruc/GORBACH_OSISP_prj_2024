#include "FILE_TYPES.h"
#include <ncurses.h>

List* read_dir(char* path) {
    DIR *dir;
    struct stat st;
    struct dirent* d;
    List* head = NULL, *tail = NULL;

    dir = opendir(path);
    while((d = readdir(dir))) {
        if(strcmp(d->d_name, ".") == 0)
            continue;
        add_last(&head, &tail, d->d_name);
    }

    closedir(dir);
    return head;
}