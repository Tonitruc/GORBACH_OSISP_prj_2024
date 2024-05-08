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

bool cpyfile(wchar_t* file_path, wchar_t* new_dir) {
    char* fp = wchtochs(file_path);
    char* nd = wchtochs(new_dir);

    int ind_fp;
    if((ind_fp = open(fp, O_RDONLY)) == -1) {
        return false;
    }

    int ind_nd;
    if((ind_nd = open(nd, O_CREAT | O_WRONLY | O_TRUNC, 0666)) == -1) {
        return false;
    }

    char buffer[100];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(ind_fp, buffer, 100)) > 0) {
        bytes_written = write(ind_nd, buffer, bytes_read);
        if (bytes_written == -1) {
            return false;
        }
    }

    if (bytes_read == -1) {
        return false;
    }

    free(nd);
    free(fp);
    close(ind_nd);
    close(ind_fp);

    return true;
}

bool cpydir(wchar_t* copy_dir, wchar_t* new_dir) {
    char* cd = wchtochs(copy_dir);
    char* nd = wchtochs(new_dir);

    DIR *dir;
    struct dirent* d;

    if(mkdir(nd, 0777) == -1) {
        return false;
    }

    dir = opendir(cd);
    while((d = readdir(dir))) {
        if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;

        char* next_path = (char*)calloc(strlen(d->d_name) + strlen(cd) + 2, sizeof(char));
        sprintf(next_path, "%s/%s", cd, d->d_name);
        wchar_t* wnext_path = cstowchs(next_path);

        char* new_path = (char*)calloc(strlen(d->d_name) + strlen(nd) + 2, sizeof(char));
        sprintf(next_path, "%s/%s", nd, d->d_name);
        wchar_t* wnew_path = cstowchs(new_path);


        if(get_file_type(wnext_path) == DIRECTORY) {
            cpydir(wnext_path, wnew_path);
        } else {
            cpyfile(wnext_path, wnew_path);
        }

        free(next_path);
        free(wnext_path);
        free(new_path);
        free(wnew_path);
    }

    free(cd);
    free(nd);

    closedir(dir);

    return true;
}
