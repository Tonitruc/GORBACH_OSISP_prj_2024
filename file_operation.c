#include "file_operation.h"

int delete_file(wchar_t* full_path) {
    char* buffer = wchtochs(full_path);
    int status = unlink(buffer);
    free(buffer);

    return status;
}

bool create_directory(wchar_t* full_path) {
    char* buffer = wchtochs(full_path);

    if(mkdir(buffer, 0777) == 0) {
        return true;
    } 

    free(buffer);
    return false;
}

bool create_file(wchar_t* full_path) {
    char* buffer = wchtochs(full_path);

    FILE *file = fopen(buffer, "w");
    if(file == NULL) {
        return false;
    }

    free(buffer);
    return true;
}

bool move_file(wchar_t* file_path, wchar_t* new_dir) {
    bool status = false;
    char* file_path_buffer = wchtochs(file_path);
    char* new_dir_buffer = wchtochs(new_dir);

    if(rename(file_path_buffer, new_dir_buffer) == 0) {
        status = true;
    }

    return status;
}

bool copy_file(wchar_t* file_path, wchar_t* new_dir) {
    return true;
}
