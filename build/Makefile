# Don't use normal gcc, use the arm cross compiler
CC = arm-none-eabi-gcc
OBJCOPY = $(TOOLCHAIN)-objcopy
GDB = $(TOOLCHAIN)-gdb
TOOLCHAIN = ./gcc-arm-none-eabi-6-2017-q2-update/bin/arm-none-eabi

# Set any constants based on the raspberry pi model.  Version 1 has some differences to 2 and 3
CPU = cortex-a7
ARCHDIR = model2


CFLAGS= -mcpu=$(CPU) -fpic -ffreestanding $(DIRECTIVES)
CSRCFLAGS= -O2 -Wall -Wextra
LFLAGS= -ffreestanding -O2 -nostdlib

# Location of the files
KER_SRC = ../src/kernel
KER_HEAD = ../include
COMMON_SRC = ../src/common
OBJ_DIR = objects
KERSOURCES = $(wildcard $(KER_SRC)/*.c)
KERSOURCES += $(wildcard $(KER_SRC)/$(ARCHDIR)/*.c)
COMMONSOURCES = $(wildcard $(COMMON_SRC)/*.c)
ASMSOURCES = $(wildcard $(KER_SRC)/*.S)
OBJECTS = $(patsubst $(KER_SRC)/%.c, $(OBJ_DIR)/%.o, $(KERSOURCES))
OBJECTS += $(patsubst $(COMMON_SRC)/%.c, $(OBJ_DIR)/%.o, $(COMMONSOURCES))
OBJECTS += $(patsubst $(KER_SRC)/%.S, $(OBJ_DIR)/%.o, $(ASMSOURCES))
HEADERS = $(wildcard $(KER_HEAD)/*.h)

IMG_NAME=kernel.img


build: $(OBJECTS) $(HEADERS)
	echo $(OBJECTS)
	$(CC) -T linker.ld -o $(IMG_NAME) $(LFLAGS) $(OBJECTS)

$(OBJ_DIR)/%.o: $(KER_SRC)/%.c
	if not exist $(@D) mkdir $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -I$(KER_HEAD) -c $< -o $@ $(CSRCFLAGS)

$(OBJ_DIR)/%.o: $(KER_SRC)/$(ARCHDIR)/%.c
	if not exist $(@D) mkdir $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -I$(KER_HEAD) -c $< -o $@ $(CSRCFLAGS)


$(OBJ_DIR)/%.o: $(KER_SRC)/%.S
	if not exist $(@D) mkdir $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -c $< -o $@

$(OBJ_DIR)/%.o: $(COMMON_SRC)/%.c
	if not exist $(@D) mkdir $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -I$(KER_HEAD) -c $< -o $@ $(CSRCFLAGS)

clean:
	setlocal enableextensions
	rm -rf $(OBJ_DIR)
	if exist "$(IMG_NAME)" rm $(IMG_NAME)
	mkdir $(OBJ_DIR)
	mkdir $(OBJ_DIR)\$(ARCHDIR)

run: build
	qemu-system-arm -m 1024 -no-reboot -M raspi2b -serial stdio -kernel kernel.img