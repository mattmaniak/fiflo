TARGET=fiflo

SDIR=src
ODIR=obj
MDIR=man
BDIR=/usr/bin

ifeq ($(BDIR)/gcc, $(shell ls $(BDIR)/gcc))
CC=gcc
CFLAGS=-std=gnu99 -O3 -Wall -Wextra

else ifeq ($(BDIR)/clang, $(shell ls $(BDIR)/clang))
CC=clang
CFLAGS=-std=gnu99 -O3 -Weverything

else
$(error Compiler not found: gcc or clang is required.)
endif

DEPS=$(TARGET).h
OBJ=$(ODIR)/$(TARGET).o $(ODIR)/keys.o $(ODIR)/logic.o $(ODIR)/render.o

# Compilation of object files depends on source files wnich depends on headers.
# "$@" - alias to name at the left of ':', "$^" - right.
# "$<" is a first item in dependencies list.
$(ODIR)/%.o: $(SDIR)/%.c $(SDIR)/$(DEPS)
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

sanitize: $(OBJ)
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

