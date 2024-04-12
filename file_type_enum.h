#pragma once

typedef enum _FILE_TYPES {
    UNKNOWN_FILE,
    DIRECTORY,
    REGULAR_FILE,
    SYMBOL_LINK,
    EXE,
    IMAGE,
    PDF,
    DOC,
    ARCHIVE,
    
} FILE_TYPE;