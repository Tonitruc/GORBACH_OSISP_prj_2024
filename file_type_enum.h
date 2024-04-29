#pragma once

typedef enum _FILE_TYPES {
    UNKNOWN_FILE = 0,
    REGULAR_FILE = 1,
    SYMBOL_LINK = 2,
    EXE = 3,
    IMAGE = 4,
    PDF = 5,
    DOC = 6,
    ARCHIVE = 7,
    DIRECTORY = 10,
    
} FILE_TYPE;