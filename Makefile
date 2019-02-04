TARGET = fiflo

CC =
CFLAGS = -std=c11
LDFLAGS =

GCOV_FLAGS = -ftest-coverage -fprofile-arcs

ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -fsanitize=leak \
-fsanitize-address-use-after-scope -fsanitize-undefined-trap-on-error \
-fstack-protector-all

MSAN_FLAGS = -fsanitize=memory -fPIE -fno-omit-frame-pointer \
-fsanitize-memory-track-origins

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
COV_DIR = cov
MAN_DIR = man
DOC_DIR = doc

USR_INS_DIR = /usr/bin
DOC_INS_DIR = /usr/share/doc/$(TARGET)
MAN_INS_DIR = /usr/share/man/man
CONF_DIR = ~/.config

# All in the ./obj depending on the ./src.
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

# Check and set the Compilation driver.
ifeq ($(USR_INS_DIR)/gcc, $(shell ls $(USR_INS_DIR)/gcc))
	CC = gcc
	CFLAGS += -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wno-long-long \
	-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wconversion \
	-Winline -Wredundant-decls -Wnested-externs -Wcast-align -Wstrict-prototypes

else ifeq ($(USR_INS_DIR)/clang, $(shell ls $(USR_INS_DIR)/clang))
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
.PHONY: $(TARGET)
$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$@ $^ \
	$(CFLAGS) $(LDFLAGS)

	@echo ' '
	@echo "Fiflo compiled."

# Debugging.
.PHONY: debug
debug: CFLAGS += $(GCOV_FLAGS)
debug: LDFLAGS += $(ASAN_FLAGS)
debug: clean
debug: $(TARGET)
debug:
	@echo "ASan linked. Files for the gcov created."
	$(RM) $(OBJ_DIR)/*.o
# Prevent $(TARGET) compilation errors.

.PHONY: coverage
coverage:
	mkdir -p $(COV_DIR)
	@echo ' '
	gcov $(OBJ_DIR)/*.gcno
	mv *.gcov $(COV_DIR)

.PHONY: memory
memory: CC = clang
memory: CFLAGS = -Weverything
memory: LDFLAGS += $(MSAN_FLAGS)
memory: clean
memory: $(TARGET)
memory:
	@echo "MSan linked."

# Fun with a filesystem.
.PHONY: install
install: $(TARGET)
	@echo ' '
	sudo cp $(BIN_DIR)/$(TARGET) $(USR_INS_DIR)/$(TARGET)

	sudo $(RM) $(MAN_INS_DIR)1/$(TARGET).1.gz
	sudo $(RM) $(MAN_INS_DIR)5/$(TARGET)rc.5.gz

	sudo cp $(MAN_DIR)/$(TARGET).1 $(MAN_INS_DIR)1
	sudo cp $(MAN_DIR)/$(TARGET)rc.5 $(MAN_INS_DIR)5

	sudo gzip $(MAN_INS_DIR)1/$(TARGET).1
	sudo gzip $(MAN_INS_DIR)5/$(TARGET)rc.5

	sudo $(RM) -r $(DOC_INS_DIR)
	sudo mkdir $(DOC_INS_DIR)
	sudo cp LICENSE $(DOC_INS_DIR)
	sudo cp $(DOC_DIR)/* $(DOC_INS_DIR)

	cp -i $(TARGET)rc $(CONF_DIR)/$(TARGET)rc

	@echo " "
	@echo "Fiflo installed."

.PHONY: install_debug
install_debug: debug
	@echo " "
	sudo cp $(BIN_DIR)/$(TARGET) $(USR_INS_DIR)/$(TARGET)

	@echo " "
	@echo "Binary only installation with ASan and gdb/gcov support finished."

.PHONY: uninstall
uninstall:
	sudo $(RM) $(USR_INS_DIR)/$(TARGET)

	sudo $(RM) -r \
	$(MAN_INS_DIR)1/$(TARGET).1.gz \
	$(MAN_INS_DIR)5/$(TARGET)rc.5.gz \
	$(DOC_INS_DIR) \
	$(CONF_DIR)/$(TARGET)rc

	@echo " "
	@echo "Fiflo uninstalled."

.PHONY: clean
clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR) $(COV_DIR)
