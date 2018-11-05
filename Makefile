TARGET = fiflo

SDIR = src
ODIR = obj
MDIR = man
BDIR = /usr/bin

CFLAGS = -std=gnu99 -O3

ifeq ($(BDIR)/gcc, $(shell ls $(BDIR)/gcc))
CC = gcc
CFLAGS += -Wall -Wextra

else ifeq ($(BDIR)/clang, $(shell ls $(BDIR)/clang))
CC = clang
CFLAGS += -Weverything

else
$(error Compiler not found: gcc or clang is required.)
endif

DEPS = $(TARGET).h
OBJ = $(ODIR)/$(TARGET).o $(ODIR)/keys.o $(ODIR)/logic.o $(ODIR)/render.o

# Compilation of object files depends on source files wnich depends on headers.
# "$@" - alias to name on the left of ':', "$^" - on the right.
# "$<" is a first item in the dependencies list.
# "-c" generates the object file.
$(ODIR)/%.o: $(SDIR)/%.c $(SDIR)/$(DEPS)
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

memory: $(OBJ)
	$(CC) -o $(TARGET) $^ $(CFLAGS) -fsanitize=address

install:
	sudo cp $(TARGET) $(BDIR)/$(TARGET)
	sudo cp $(MDIR)/$(TARGET).1 /usr/share/man/man1/$(TARGET).1
	sudo gzip /usr/share/man/man1/$(TARGET).1

uninstall:
	sudo $(RM) $(BDIR)/$(TARGET) /usr/share/man/man1/$(TARGET).1.gz

.PHONY: clean

clean:
	$(RM) $(TARGET) -r $(ODIR)

