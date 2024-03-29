#pragma once

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "List.h"

typedef enum _FILE_TYPES {
    NON_FILE,
    DIRECTORY,
    REGULAR_FILE,
    SYMBOL_LINK,
} FILE_TYPE;

 List* read_dir(char* path);
