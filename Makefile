TARGET = fiflo

CC =
CFLAGS = -std=c11 -Os
LDFLAGS =

ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -fsanitize=leak \
-fsanitize-address-use-after-scope -fsanitize-undefined-trap-on-error \
-fstack-protector-all

MSAN_FLAGS = -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer \
-fsanitize-memory-track-origins

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
COV_DIR = cov
MAN_DIR = man

INS_DIR = /usr/bin
MAN_INS_DIR = /usr/share/man/man
CONF_DIR = ~/.config

# All in the ./obj depending on the ./src.
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

# Check and set the Compilation driver.
ifeq ($(INS_DIR)/gcc, $(shell ls $(INS_DIR)/gcc))
	CC = gcc
	CFLAGS += -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wno-long-long \
	-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wconversion \
	-Winline -Wredundant-decls -Wnested-externs -Wcast-align -Wstrict-prototypes

else ifeq ($(INS_DIR)/clang, $(shell ls $(INS_DIR)/clang))
	CC = clang
	CFLAGS += -Weverything

else
	$(error Compilation driver was not found: gcc or clang is required.)
endif

# Compilation of object files depends on source files wnich depends on headers.
# "$@" - alias to name on the left of ':', "$^" - on the right.
# "$<" is a first item in the dependencies list.
# "-c" generates the object file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	mkdir -p $(OBJ_DIR)
	$(CC) -c -o $@ $< \
	$(CFLAGS) \

# Builds the binary by linking object files.
$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$@ $^ \
	$(CFLAGS) $(LDFLAGS)

	@echo ' '
	@echo "Compilation successful."

# Debugging.
.PHONY: address
address: LDFLAGS += $(ASAN_FLAGS)
address: $(TARGET)
address:
	@echo "AddressSanitizer linked successfuly."

.PHONY: debug
debug: CFLAGS += -g -ftest-coverage -fprofile-arcs
debug: LDFLAGS += $(ASAN_FLAGS)
debug: clean
debug: $(TARGET)
debug:
	@echo "AddressSanitizer linked successfuly. Files for the gcov created."

.PHONY: coverage
coverage:
	mkdir -p $(COV_DIR)
	@echo ' '
	gcov $(OBJ_DIR)/*.gcno
	mv *.gcov $(COV_DIR)

# TODO
.PHONY: memory
memory: CC = clang
memory: CFLAGS = -Weverything
memory: LDFLAGS += $(MSAN_FLAGS)
memory: $(TARGET)
memory:
	@echo "MemorySanitizer linked successfuly."

# Fun with a filesystem.
.PHONY: install
install: $(TARGET)
	@echo ' '
	sudo cp $(BIN_DIR)/$(TARGET) $(INS_DIR)/$(TARGET)

	sudo $(RM) $(MAN_INS_DIR)1/$(TARGET).1.gz
	sudo $(RM) $(MAN_INS_DIR)5/$(TARGET)rc.5.gz

	sudo cp $(MAN_DIR)/$(TARGET).1 $(MAN_INS_DIR)1/$(TARGET).1
	sudo cp $(MAN_DIR)/$(TARGET)rc.5 $(MAN_INS_DIR)5/$(TARGET)rc.5

	sudo gzip $(MAN_INS_DIR)1/$(TARGET).1
	sudo gzip $(MAN_INS_DIR)5/$(TARGET)rc.5

	cp -i $(TARGET)rc $(CONF_DIR)/$(TARGET)rc

	@echo " "
	@echo "Fiflo installation finished."

.PHONY: install_address
install_address: address
	@echo " "
	sudo cp $(BIN_DIR)/$(TARGET) $(INS_DIR)/$(TARGET)

	@echo " "
	@echo "Fiflo binary only installation with ASan finished."

.PHONY: uninstall
uninstall:
	sudo $(RM) \
	$(INS_DIR)/$(TARGET) \
	$(MAN_INS_DIR)1/$(TARGET).1.gz \
	$(MAN_INS_DIR)5/$(TARGET)rc.5.gz \
	$(CONF_DIR)/$(TARGET)rc

	@echo " "
	@echo "Fiflo uninstallation finished."

.PHONY: clean
clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR) $(COV_DIR)
