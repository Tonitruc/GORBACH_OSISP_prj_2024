#include "file_operation.h"
#include <ncurses.h>

FOPR delete_file(wchar_t* full_path, bool is_dir) {   //Функция удаления файла или каталога 
    char* fp = wchtochs(full_path);
    FOPR status = SUCCESS;

    if(is_dir) {
        DIR *dir;
        struct dirent* d;

        dir = opendir(fp);  //Открытие каталога 
        if(dir == NULL) {
            return SDIR_NOT_EXIST;  //Если не существует 
        }
        while((d = readdir(dir)) && status == SUCCESS) {   
            if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
                continue;

            char* next_path = (char*)calloc(strlen(d->d_name) + strlen(fp) + 2, sizeof(char));
            sprintf(next_path, "%s/%s", fp, d->d_name);
            wchar_t* wnext_path = cstowchs(next_path);

            if(get_file_type(wnext_path) == DIRECTORY) {  //Если это каталог 
                status = delete_file(wnext_path, true); //То рекурсивный вызов
            } else {
                status = delete_file(wnext_path, false);   //Удаление только файла
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
        if(unlink(fp) == -1) {  //Удаление файла
            status = OPERATION_ERROR;
        }
    }
    free(fp);

    return status;
}

FOPR create_directory(wchar_t* full_path) {  //Создание каталога 
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

FOPR rnm_file(wchar_t* path, wchar_t* new_name) {   //Изменение имени файла 
    size_t ps = wcslen(path);
    while(ps > 0 && path[ps - 1] != '/') {    //Получение исходного каталога 
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

    int status = rename(op, np);    //Изменение имени файла 
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

FOPR crt_file(wchar_t* full_path) {  //Создание файла 
    char* fp = wchtochs(full_path);

    int status = open(fp, O_CREAT, 0777);  //Создать файл
    if(status == -1) {
        if(errno == EEXIST) {
            return NAME_EXIST;
        }
        return OPERATION_ERROR;
    }

    free(fp);
    close(status);
    return SUCCESS;
}

FOPR cpyfile(wchar_t* file_path, wchar_t* new_dir, int save_atr) {  //Копирование файлов 
    char* fp = wchtochs(file_path);
    char* nd = wchtochs(new_dir);

    int ind_fp;
    if((ind_fp = open(fp, O_RDONLY)) == -1) {  //Открытие исходного файла 
        return SDIR_NOT_EXIST;
    }

    int ind_nd;
    if((ind_nd = open(nd, O_CREAT | O_WRONLY | O_TRUNC, 0777)) == -1) {  //Создание результирующего файла 
        if(errno == EEXIST) {
            return NAME_EXIST;
        }
        return RDIR_NOT_EXITS;
    }

    char buffer[100];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(ind_fp, buffer, 100)) > 0) {  //Чтение информации из исходного файла 
        bytes_written = write(ind_nd, buffer, bytes_read);
        if (bytes_written == -1) {
            return OPERATION_ERROR;
        }
    }

    if (bytes_read == -1) {  
        return OPERATION_ERROR;
    }

    if(save_atr == 1) {  //Сохранение исходных атрибутов файла 
        struct stat st;
        stat(fp, &st);
        chmod(nd, st.st_mode);
    }

    free(nd);
    free(fp);
    close(ind_nd);
    close(ind_fp);

    return SUCCESS;
}

FOPR cpyslnk(wchar_t* copy_link, wchar_t* new_dir, int save_attr) {  //Копирование символьной ссылки
    wchar_t* path = find_slnk_path(copy_link);   //Получение пути к файлу куда указывает символьная ссылка 
    size_t size = wcslen(copy_link);
    while(size > 0 && copy_link[size - 1] != '/') {
        size--;
    }
    wchar_t* name = wsubstring(copy_link, size + 1, wcslen(copy_link) - size);
    size = wcslen(new_dir) + wcslen(name) + 2;
    wchar_t* next_path = (wchar_t*)calloc(size, sizeof(wchar_t));
    swprintf(next_path, size, L"%ls/%ls", new_dir, name);

    FOPR status = create_slnk(path, next_path);  // Создание символьной ссылки 

    if(save_attr == 1) {  //Сохранение исходных атрибутов 
        struct stat st;
        char* copy = wchtochs(copy_link);
        stat(copy, &st);
        char* buffer = wchtochs(next_path);
        chmod(buffer, st.st_mode);
        free(buffer);
        free(copy);
    }

    free(next_path);
    free(path);
    free(name);

    return status;
}

FOPR cpydir(wchar_t* copy_dir, wchar_t* new_dir, int save_attr, int link) {  //Копироваине каталога 
    char* cd = wchtochs(copy_dir);
    char* nd = wchtochs(new_dir);

    DIR *dir;
    struct dirent* d;
    FOPR status = SUCCESS;
    int st = mkdir(nd, 0777);  //Создание каталога 
    if(st == -1) {
        if(errno == EEXIST) {
            return NAME_EXIST;
        }
        if(save_attr == 1) {  //Сохранение исходных атрибутов 
            struct stat st;
            stat(cd, &st);
            chmod(nd, st.st_mode);
        }
    }

    dir = opendir(cd);
    if(dir == NULL) {
        return SDIR_NOT_EXIST;
    }
    while((d = readdir(dir)) && status == SUCCESS) {  //Чтение каталога 
        if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;

        char* next_path = (char*)calloc(strlen(d->d_name) + strlen(cd) + 2, sizeof(char));
        sprintf(next_path, "%s/%s", cd, d->d_name);
        wchar_t* wnext_path = cstowchs(next_path);

        char* new_path = (char*)calloc(strlen(d->d_name) + strlen(nd) + 2, sizeof(char));
        sprintf(new_path, "%s/%s", nd, d->d_name);
        wchar_t* wnew_path = cstowchs(new_path);

        if(get_file_type(wnext_path) == DIRECTORY) {  //Если следующий файл является каталогом 
            status = cpydir(wnext_path, wnew_path, save_attr, link);
        }
        else if(get_file_type(wnew_path) == SYMBOL_LINK && link == -1) {  //Если символьной ссылкой 
            status = cpyslnk(wnext_path, new_dir, save_attr);
        } 
        else {
            status = cpyfile(wnext_path, wnew_path, save_attr);  //Любой другой файл 
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

FOPR create_slnk(wchar_t* dir, wchar_t* name) {  //Создание символьной ссылки 
    char* d = wchtochs(dir);
    char* n = wchtochs(name);

    int status = symlink(d, n);  //Создание ссылки с именем 
    if(status == -1) {
        if(errno == EEXIST) {
            status = NAME_EXIST;
        } else {
            status = OPERATION_ERROR;
        }
    } else {
        status = SUCCESS;
    }

    free(d);
    free(n);

    return status;
}

wchar_t* find_slnk_path(wchar_t* slnk) {  //Получение пути куда указывает символьная ссылка 
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