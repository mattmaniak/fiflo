TARGET = fiflo

CC =
CFLAGS = -std=c11 -Os
DEBUGFLAGS =

ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -fsanitize=leak \
-fsanitize-address-use-after-scope -fsanitize-undefined-trap-on-error \
-fstack-protector-all

MSAN_FLAGS = -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer \
-fsanitize-memory-track-origins

SRC_DIR = src
INC_DIR = $(SRC_DIR)/include
OBJ_DIR = obj
BIN_DIR = bin
MAN_DIR = man

INSTALL_DIR = /usr/bin
MAN_INSTALL_DIR = /usr/share/man/man1

# All in the ./obj depending on the ./src.
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

# Check and set the Compilation driver.
ifeq ($(INSTALL_DIR)/gcc, $(shell ls $(INSTALL_DIR)/gcc))
	CC = gcc
	CFLAGS += -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wno-long-long \
	-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wconversion \
	-Winline -Wredundant-decls -Wnested-externs -Wcast-align -Wstrict-prototypes

else ifeq ($(INSTALL_DIR)/clang, $(shell ls $(INSTALL_DIR)/clang))
	CC = clang
	CFLAGS += -Weverything

else
	$(error Compilation driver was not found: gcc or clang is required.)
endif

# Compilation of object files depends on source files wnich depends on headers.
# "$@" - alias to name on the left of ':', "$^" - on the right.
# "$<" is a first item in the dependencies list.
# "-c" generates the object file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	@mkdir -p $(OBJ_DIR)
	$(CC) -c -o $@ $< \
	$(CFLAGS) \

# Builds the binary by linking object files.
$(TARGET): $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$@ $^ \
	$(CFLAGS) \
	$(DEBUGFLAGS)

# Debugging.
asan: DEBUGFLAGS = $(ASAN_FLAGS)
asan: $(TARGET)

msan: CC = clang
msan: DEBUGFLAGS = $(MSAN_FLAGS)
msan: $(TARGET)

valgrind: $(TARGET)
valgrind:
	valgrind -v ./$(BIN_DIR)/$(TARGET) $(FIFLO_ARG) # Dynamic variable.

# Fun with a filesystem.
install: $(TARGET)
	sudo cp $(BIN_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)
	sudo $(RM) $(MAN_INSTALL_DIR)/$(TARGET).1
	sudo cp $(MAN_DIR)/$(TARGET).1 $(MAN_INSTALL_DIR)/$(TARGET).1
	sudo gzip $(MAN_INSTALL_DIR)/$(TARGET).1

uninstall:
	sudo $(RM) \
	$(INSTALL_DIR)/$(TARGET) \
	$(MAN_INSTALL_DIR)/$(TARGET).1.gz

.PHONY: clean

clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR)
