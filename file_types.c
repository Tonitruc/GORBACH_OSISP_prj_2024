#include "file_types.h"

wchar_t* get_cur_dir() {  //Получение текущего каталога 
    size_t size = 1024;
    char* buffer = (char*)calloc(size, sizeof(char)); 
    wchar_t* cur_dir = NULL;

    bool is_break = false;
    while(!is_break) {
        if (getcwd(buffer, size) != NULL) {
            is_break = true;
            cur_dir = cstowchs(buffer);
        } 
        else if(errno == ERANGE) {
            size *= 2;
            buffer = (char*)realloc(buffer, size * sizeof(char));
        } 
        else {
            is_break = true;
        }
    }
    free(buffer);

    return cur_dir;
}
//Создание структуры file_info
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

FILE_TYPE get_file_type(wchar_t* wfull_path) {  //Получение типа файла 
    char* full_path = wchtochs(wfull_path);
    struct stat file_info;
    FILE_TYPE file_type;

    if (lstat(full_path, &file_info) == 0) {  //Получение информации о файле
        if (S_ISDIR(file_info.st_mode)) { file_type = DIRECTORY; }
        else if (S_ISLNK(file_info.st_mode)) { 
            if(access(full_path, F_OK) == 0) {
                file_type = SYMBOL_LINK; 
            } else {
                file_type = BAD_SYMBOL_LINK;
            }
            } 
        else if (wchstrcmp(wfull_path, L".png", 0, 0)  //Парсинг формата файла 
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

LIST* read_dir(char* path) {  //Чтение каталога и сохранение информации о его файлах 
    DIR *dir;
    struct dirent* d;
    struct stat file_info;

    LIST* list = init_list();
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

        if (stat(buffer, &file_info) == 0) { //Получение информации о файле 
            edit_time = file_info.st_mtime;
            size_kb = file_info.st_size;
        }

        wbuffer = cstowchs(buffer);
        file_type = get_file_type(wbuffer); //Сохранение информации о файле в структуре 
        finfo = init_file_info(full_path, file_name, edit_time, size_kb, file_type);

        add_last(list, finfo);
        free(buffer);
        free(file_name);
        free(full_path);
        free(wbuffer);
    }

    closedir(dir);
    return list;
}

int finfo_name_compare(FINFO* first, FINFO* second, int dir) { //Функция для сравнения файлов по имени с каталогами в приоритете 
    if(first->file_type == DIRECTORY && second->file_type != DIRECTORY) {
        return dir == 0? -1 : 1;
    } 
    else if(first->file_type != DIRECTORY && second->file_type == DIRECTORY) {
        return dir == 0? 1 : -1;
    } 
    else {
        return wcscmp(first->file_name, second->file_name);
    }
}

int finfo_size_compare(FINFO* first, FINFO* second, int dir) { //Функция для сравнения файлов по размеру с каталогами в приоритете 
    if(first->file_type == DIRECTORY && second->file_type != DIRECTORY) {
        return dir == 0? -1 : 1;
    } 
    else if(first->file_type != DIRECTORY && second->file_type == DIRECTORY) {
        return dir == 0? 1 : -1;
    } 
    else {
        return first->size_kb <= second->size_kb? 1 : -1;
    }
}

int finfo_time_compare(FINFO* first, FINFO* second, int dir) { //Функция для сравнения файлов по времени правки с каталогами в приоритете 
    if(first->file_type == DIRECTORY && second->file_type != DIRECTORY) {
        return dir == 0? -1 : 1;
    } 
    else if(first->file_type != DIRECTORY && second->file_type == DIRECTORY) {
        return dir == 0? 1 : -1;
    } 
    else {
        return first->edit_time <= second->edit_time? 1 : -1;
    }
}

void rfind_files(LIST* result, char* start_dir, regex_t regex) {  //Поиск файлов по регулярному выражению 
    DIR *dir;
    struct dirent* d;
    struct stat file_info;

    FINFO* finfo;
    wchar_t* file_name; wchar_t* full_path;
    time_t edit_time; long size_kb;
    FILE_TYPE file_type;

    wchar_t* wbuffer;
    dir = opendir(start_dir);

    stat(start_dir, &file_info);
    wchar_t* wstart_dir = cstowchs(start_dir);
    finfo = init_file_info(wstart_dir, wstart_dir, file_info.st_ctime, file_info.st_size, DIRECTORY);
    add_last(result, finfo);

    while((d = readdir(dir))) {
        if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;

        char* buffer = (char*)calloc(strlen(d->d_name) + strlen(start_dir) + 2, sizeof(char));
        sprintf(buffer, "%s/%s", start_dir, d->d_name);
        full_path = cstowchs(buffer);

        file_name = cstowchs(d->d_name);
        wbuffer = cstowchs(buffer);
        file_type = get_file_type(wbuffer);

        if(get_file_type(full_path) == DIRECTORY) {   
            rfind_files(result, buffer, regex);
        } else if(regexec(&regex, d->d_name, 0, NULL, 0) == 0) {
            stat(buffer, &file_info);
            edit_time = file_info.st_mtime;
            size_kb = file_info.st_size;      

            finfo = init_file_info(full_path, file_name, edit_time, size_kb, file_type);
            add_last(result, finfo);
        }

        free(wbuffer);
        free(buffer);
        free(full_path);
        free(file_name);
    }
    char* buffer = wchtochs(wstart_dir);
    if((result->tail) != NULL && result->tail->data->file_type == DIRECTORY 
        && regexec(&regex, buffer, 0, NULL, 0) != 0) {
        remove_last(result);
    }
    free(buffer);
    closedir(dir);
}

int find_files(LIST* result, wchar_t* wstart_dir, wchar_t* wpattern) { //Функция для поиска файла по регулярному выражению 
    wchar_t* buffer = parse_regex_pattern(wpattern); //Функция для упрощенного ввода паттерна для поиска

    regex_t reg;
    char* pattern = wchtochs(buffer);

    if(regcomp(&reg, pattern, REG_EXTENDED) != 0) {
        free(pattern);
        return -1;
    }

    char* start_dir = wchtochs(wstart_dir);
    rfind_files(result, start_dir, reg);

    free(buffer);
    free(pattern);

    return 1;
}

wchar_t* get_file_name(wchar_t* full_path) { //Парс полного пути для получения имени файла 
    size_t size = wcslen(full_path);
    int i = size;
    while(full_path[i] != '/' && i > 0) {
        i--;
    } 
    wchar_t* file_name = wsubstring(full_path, i + 2, size - i);
    return file_name;
}