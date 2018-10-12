CC=gcc
CFLAGS=-std=gnu99 -O3 -I./src -Wall -Wextra

TARGET=fiflo

SDIR=./src
ODIR=./obj
MDIR=./man

DEPS=$(SDIR)/$(TARGET).h
OBJ=$(SDIR)/$(TARGET).o $(SDIR)/keys.o $(SDIR)/api.o $(SDIR)/render.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

install:
	sudo cp $(TARGET) /usr/bin/$(TARGET)
	sudo cp $(MDIR)/$(TARGET).1 /usr/share/man/man1/$(TARGET).1
	sudo gzip /usr/share/man/man1/$(TARGET).1

uninstall:
	sudo $(RM) /usr/bin/$(TARGET) /usr/share/man/man1/$(TARGET).1.gz

.PHONY: clean

clean:
	$(RM) $(TARGET) $(SDIR)/*.o

