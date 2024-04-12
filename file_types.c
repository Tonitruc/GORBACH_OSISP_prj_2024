#include "file_types.h"

FINFO* init_file_info(wchar_t* full_path, wchar_t* file_name, time_t edit_time, int size_kb, FILE_TYPE file_type) {
    FINFO* finfo = (FINFO*)calloc(1, sizeof(FINFO));

    finfo->edit_time = edit_time;

    finfo->full_path = (wchar_t*)calloc(wcslen(full_path) + 1, sizeof(wchar_t));
    wcscpy(finfo->full_path, full_path);

    finfo->size_kb = size_kb;
    
    finfo->file_name = (wchar_t*)calloc(wcslen(file_name) + 1, sizeof(wchar_t));
    wcscpy(finfo->file_name, file_name);

    finfo->file_type = file_type;

    return finfo;
}

FILE_TYPE get_file_type(wchar_t* wfull_path) {
    char* full_path = wchtochs(wfull_path);
    struct stat file_info;
    FILE_TYPE file_type;

    if (stat(full_path, &file_info) == 0) {
        if (S_ISDIR(file_info.st_mode)) { file_type = DIRECTORY; }
        else if (S_ISLNK(file_info.st_mode)) { file_type = SYMBOL_LINK; }
        else if (wchstrcmp(wfull_path, L".png", 0, 0) 
                || wchstrcmp(wfull_path, L".jpg", 0, 0)) {file_type = IMAGE; } 
        else if (wchstrcmp(wfull_path, L".odt", 0, 0) 
                || wchstrcmp(wfull_path, L".docx", 0, 0)) {file_type = DOC; } 
        else if (wchstrcmp(wfull_path, L".pdf", 0, 0)) {file_type = PDF; } 
        else if(wchstrcmp(wfull_path, L".zip", 0, 0) 
                || wchstrcmp(wfull_path, L".rar", 0, 0)
                || wchstrcmp(wfull_path, L".tar", 0, 0)
                || wchstrcmp(wfull_path, L".gz", 0, 0)
                || wchstrcmp(wfull_path, L".7zip", 0, 0)) {file_type = ARCHIVE; } 

        else { file_type = UNKNOWN_FILE; }
    }

    free(full_path);

    return file_type;
}

List* read_dir(char* path) {
    DIR *dir;
    struct dirent* d;
    struct stat file_info;

    List* head = NULL, *tail = NULL;
    FINFO* finfo;
    wchar_t* file_name; wchar_t* full_path;
    time_t edit_time; long size_kb;
    FILE_TYPE file_type;
    wchar_t* wbuffer;

    dir = opendir(path);
    while((d = readdir(dir))) {
        if(strcmp(d->d_name, ".") == 0)
            continue;

        file_name = cstowchs(d->d_name);
        full_path = cstowchs(path);


        char* buffer = (char*)calloc(strlen(d->d_name) + strlen(path) + 2, sizeof(char));
        sprintf(buffer, "%s/%s", path, d->d_name);

        if (stat(buffer, &file_info) == 0) {
            edit_time = file_info.st_mtime;
            size_kb = file_info.st_size;
        }

        wbuffer = cstowchs(buffer);
        file_type = get_file_type(wbuffer);
        finfo = init_file_info(full_path, file_name, edit_time, size_kb, file_type);

        add_last(&head, &tail, finfo);
        free(buffer);
        free(wbuffer);
    }

    closedir(dir);
    return head;
}