# Компиляторы
CC=gcc
CC_FLAGS=-g -Wall -Wl,-O1 -pipe -O2 -flto=2 -fno-fat-lto-objects -fuse-linker-plugin -fPIC

# Цели
TARGET=example

# Директории
SRC_DIR="/home/tonitrus/Projects/C/OSISP/OSISP-KP/KFS Professional file manager/files"
BIN_DIR="/home/tonitrus/Projects/C/OSISP/OSISP-KP/KFS Professional file manager/program"

# Исходные коды
SOURCES=$(SRC_DIR)/*.c

DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
LIBS_DIRS     = -I./include/
LIBS 		  = $(LIBS_DIRS) -lpanel -lncursesw
SED           = sed
STRIP         = strip

.PHONY: clean build
all: clean build

build: $(SOURCE)
	$(CC) $(CC_FLAGS) $(LIBS) $(SOURCES) -o $(BIN_DIR)/$(TARGET)

run:
	./$(BIN_DIR)/$(TARGET)

clean:
	$(DEL_FILE) bin/*
