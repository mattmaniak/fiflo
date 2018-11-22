TARGET = fiflo

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
MAN_DIR = man
INSTALL_DIR = /usr/bin

CFLAGS = -std=gnu99 -O3

ASAN_FLAGS = -fsanitize=address -fsanitize=undefined \
-fsanitize-address-use-after-scope

MSAN_FLAGS = -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer \
-fsanitize-memory-track-origins

DEPS = $(TARGET).h

# All in obj folder depending on the src dir.
OBJ = $(patsubst src/%.c, obj/%.o, $(wildcard src/*c))

# Check and set the compiler.
ifeq ($(INSTALL_DIR)/clang, $(shell ls $(INSTALL_DIR)/clang))
CC = clang
CFLAGS += -Weverything

else ifeq ($(INSTALL_DIR)/gcc, $(shell ls $(INSTALL_DIR)/gcc))
CC = gcc
CFLAGS += -Wall -Wextra

else
$(error Compilation driver was not found: clang or gcc is required.)
endif

# Compilation of object files depends on source files wnich depends on headers.
# "$@" - alias to name on the left of ':', "$^" - on the right.
# "$<" is a first item in the dependencies list.
# "-c" generates the object file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/$(DEPS)
	@mkdir -p $(OBJ_DIR)
	$(CC) -c -o $@ $< \
	$(CFLAGS) \

# Builds the binary by linking object files.
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$@ $^ \
	$(CFLAGS)

sanitize: $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$(TARGET) $^ \
	$(CFLAGS) \
	$(ASAN_FLAGS)

memory: $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$(TARGET) $^ \
	$(CFLAGS) \
	$(MSAN_FLAGS)

install:
	sudo cp $(BIN_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)
	sudo cp $(MAN_DIR)/$(TARGET).1 /usr/share/man/man1/$(TARGET).1
	sudo gzip /usr/share/man/man1/$(TARGET).1

uninstall:
	sudo $(RM) \
	$(INSTALL_DIR)/$(TARGET) \
	/usr/share/man/man1/$(TARGET).1.gz

.PHONY: clean

clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR)
