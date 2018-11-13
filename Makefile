TARGET = fiflo

SDIR = src
ODIR = obj
MDIR = man
BDIR = /usr/bin

CFLAGS = -std=gnu99 -O3

ifeq ($(BDIR)/clang, $(shell ls $(BDIR)/clang))
CC = clang
CFLAGS += -Weverything

else ifeq ($(BDIR)/gcc, $(shell ls $(BDIR)/gcc))
CC = gcc
CFLAGS += -Wall -Wextra

else
$(error Compiler not found: gcc or clang is required.)
endif

DEPS = $(TARGET).h

OBJ = \
$(ODIR)/$(TARGET).o \
$(ODIR)/file.o \
$(ODIR)/memory.o \
$(ODIR)/render.o \
$(ODIR)/text.o

# Compilation of object files depends on source files wnich depends on headers.
# "$@" - alias to name on the left of ':', "$^" - on the right.
# "$<" is a first item in the dependencies list.
# "-c" generates the object file.
$(ODIR)/%.o: $(SDIR)/%.c $(SDIR)/$(DEPS)
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< \
	$(CFLAGS) \

# Builds the binary by linking object files.
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ \
	$(CFLAGS)

address: $(OBJ)
	$(CC) -o $(TARGET) $^ \
	$(CFLAGS) \
	-fsanitize=address

memory: $(OBJ)
	$(CC) -o $(TARGET) $^ \
	$(CFLAGS) \
	-fsanitize=memory -fPIE -pie \
	-fno-omit-frame-pointer -fsanitize-memory-track-origins

install:
	sudo cp $(TARGET) $(BDIR)/$(TARGET)
	sudo cp $(MDIR)/$(TARGET).1 /usr/share/man/man1/$(TARGET).1
	sudo gzip /usr/share/man/man1/$(TARGET).1

uninstall:
	sudo $(RM) \
	$(BDIR)/$(TARGET) \
	/usr/share/man/man1/$(TARGET).1.gz

.PHONY: clean

clean:
	$(RM) $(TARGET) -r $(ODIR)

