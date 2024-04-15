#pragma once

typedef enum _FILE_TYPES {
    UNKNOWN_FILE = 0,
    DIRECTORY = 10,
    REGULAR_FILE = 1,
    SYMBOL_LINK,
    EXE,
    IMAGE,
    PDF,
    DOC,
    ARCHIVE,
    
} FILE_TYPE;