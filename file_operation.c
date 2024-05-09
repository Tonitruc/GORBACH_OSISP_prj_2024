#include "file_operation.h"
#include <ncurses.h>

FOPR delete_file(wchar_t* full_path, bool is_dir) {
    char* fp = wchtochs(full_path);
    FOPR status = SUCCESS;

    if(is_dir) {
        DIR *dir;
        struct dirent* d;

        dir = opendir(fp);
        if(dir == NULL) {
            return SDIR_NOT_EXIST;
        }
        while((d = readdir(dir)) && status == SUCCESS) {
            if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
                continue;

            char* next_path = (char*)calloc(strlen(d->d_name) + strlen(fp) + 2, sizeof(char));
            sprintf(next_path, "%s/%s", fp, d->d_name);
            wchar_t* wnext_path = cstowchs(next_path);

            if(get_file_type(wnext_path) == DIRECTORY) {
                status = delete_file(wnext_path, true);
            } else {
                status = delete_file(wnext_path, false);
            }

            free(next_path);
            free(wnext_path);
        }

        closedir(dir);
        if(rmdir(fp) == -1) {
            status = OPERATION_ERROR;
        }
    }
    else {
        if(unlink(fp) == -1) {
            status = OPERATION_ERROR;
        }
    }
    free(fp);

    return status;
}

FOPR create_directory(wchar_t* full_path) {
    char* buffer = wchtochs(full_path);

    int status = mkdir(buffer, 0777);
    if(status == -1) {
        if(errno == EEXIST) {
            return NAME_EXIST;
        }
        return OPERATION_ERROR;
    } 

    free(buffer);
    return SUCCESS;
}

FOPR rnm_file(wchar_t* path, wchar_t* new_name) {
    size_t ps = wcslen(path);
    while(ps > 0 && path[ps - 1] != '/') {
        ps--;
    }

    wchar_t* copy_path = (wchar_t*)calloc(ps + 1, sizeof(wchar_t));
    wcsncpy(copy_path, path, ps + 1);
    copy_path[ps] = '\0';
    ps += wcslen(new_name); 
    wchar_t* new_path = (wchar_t*)calloc(ps + 2, sizeof(wchar_t));
    swprintf(new_path, ps + 2, L"%ls%ls", copy_path, new_name);

    char* np = wchtochs(new_path);
    char* op = wchtochs(path);

    int status = rename(op, np);
    if(status == -1) {
        if(errno == EEXIST) {
            status = NAME_EXIST;
        } else {
            status = OPERATION_ERROR;
        }
    }

    free(np);
    free(op);
    free(new_path);
    return SUCCESS;
}

FOPR create_file(wchar_t* full_path) {
    char* buffer = wchtochs(full_path);

    FILE *file = fopen(buffer, "w");
    if(file == NULL) {
        return false;
    }

    free(buffer);
    return true;
}

FOPR move_file(wchar_t* file_path, wchar_t* new_dir) {
    bool status = false;
    char* file_path_buffer = wchtochs(file_path);
    char* new_dir_buffer = wchtochs(new_dir);

    if(rename(file_path_buffer, new_dir_buffer) == 0) {
        status = true;
    }

    return status;
}

FOPR cpyfile(wchar_t* file_path, wchar_t* new_dir) {
    char* fp = wchtochs(file_path);
    char* nd = wchtochs(new_dir);

    int ind_fp;
    if((ind_fp = open(fp, O_RDONLY)) == -1) {
        return SDIR_NOT_EXIST;
    }

    int ind_nd;
    if((ind_nd = open(nd, O_CREAT | O_WRONLY | O_TRUNC, 0666)) == -1) {
        if(errno == EEXIST) {
            return NAME_EXIST;
        }
        return RDIR_NOT_EXITS;
    }

    char buffer[100];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(ind_fp, buffer, 100)) > 0) {
        bytes_written = write(ind_nd, buffer, bytes_read);
        if (bytes_written == -1) {
            return OPERATION_ERROR;
        }
    }

    if (bytes_read == -1) {
        return OPERATION_ERROR;
    }

    free(nd);
    free(fp);
    close(ind_nd);
    close(ind_fp);

    return SUCCESS;
}

FOPR cpydir(wchar_t* copy_dir, wchar_t* new_dir) {
    char* cd = wchtochs(copy_dir);
    char* nd = wchtochs(new_dir);

    DIR *dir;
    struct dirent* d;
    FOPR status = SUCCESS;
    int st = mkdir(nd, 0777);
    if(st == -1) {
        if(errno == EEXIST) {
            return NAME_EXIST;
        }
    }

    dir = opendir(cd);
    if(dir == NULL) {
        return SDIR_NOT_EXIST;
    }
    while((d = readdir(dir)) && status == SUCCESS) {
        if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;

        char* next_path = (char*)calloc(strlen(d->d_name) + strlen(cd) + 2, sizeof(char));
        sprintf(next_path, "%s/%s", cd, d->d_name);
        wchar_t* wnext_path = cstowchs(next_path);

        char* new_path = (char*)calloc(strlen(d->d_name) + strlen(nd) + 2, sizeof(char));
        sprintf(new_path, "%s/%s", nd, d->d_name);
        wchar_t* wnew_path = cstowchs(new_path);

        if(get_file_type(wnext_path) == DIRECTORY) {
            status = cpydir(wnext_path, wnew_path);
        } else {
            status = cpyfile(wnext_path, wnew_path);
        }

        free(next_path);
        free(wnext_path);
        free(new_path);
        free(wnew_path);
    }

    free(cd);
    free(nd);

    closedir(dir);

    return status;
}

FOPR create_slnk(wchar_t* dir, wchar_t* name) {
    char* d = wchtochs(dir);
    char* n = wchtochs(name);

    int status = symlink(d, n);
    if(status == -1) {
        if(errno == EEXIST) {
            status = NAME_EXIST;
        } else {
            status = OPERATION_ERROR;
        }
    }

    free(d);
    free(n);

    return status;
}

wchar_t* find_slnk_path(wchar_t* slnk) {
    char* link = wchtochs(slnk);

    size_t ps = 1000;
    bool is_break = false;
    char *target_path;
    while(!is_break) {
        target_path = (char*)calloc(ps, sizeof(char));
        ssize_t len = readlink(link, target_path, ps - 1);
        if (len != -1) {
            target_path[len] = '\0'; 
            is_break = true;
        } else if(errno == ENAMETOOLONG) {
            free(target_path);
            ps *= 2;
        } else {
            free(link);
            free(target_path);
            return NULL;
        }
    }

    wchar_t* path = cstowchs(target_path);

    free(link);
    free(target_path);
    return path;
}