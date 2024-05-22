#pragma once

//____________ Библиотека для выполнения операций над файлами________________

#include <fcntl.h>

#include "file_type_enum.h"
#include "file_types.h"
#include "fwchar.h"

typedef enum _FILE_OPERATION_RESULT {  //Переменная для хранения результата файловой операции
    SDIR_NOT_EXIST = 3,
    RDIR_NOT_EXITS = 2,
    OPERATION_ERROR = 0,
    SUCCESS = 1,
    NAME_EXIST = 4,
    EMPTY_NAME = 5,
} FOPR;

//Обычный файлы 
FOPR delete_file(wchar_t* full_path, bool is_dir);
FOPR crt_file(wchar_t* full_path);
FOPR cpyfile(wchar_t* file_path, wchar_t* new_dir, int save_atr);

//Символьный ссылки 
FOPR create_slnk(wchar_t* dir, wchar_t* name);
wchar_t* find_slnk_path(wchar_t* slnk);
FOPR cpyslnk(wchar_t* copy_link, wchar_t* new_dir, int save_attr);

//Каталоги 
FOPR create_directory(wchar_t* full_path);
FOPR cpydir(wchar_t* copy_dir, wchar_t* new_dir, int save_atr, int link);


FOPR rnm_file(wchar_t* path, wchar_t* new_name);