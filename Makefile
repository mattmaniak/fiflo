CC=gcc
CFLAGS=-std=gnu99 -O3 -I./src -Wall -Wextra

TARGET=fiflo

SDIR=./src
ODIR=./obj

DEPS=$(SDIR)/$(TARGET).h
OBJ=$(SDIR)/$(TARGET).o $(SDIR)/keys.o $(SDIR)/api.o $(SDIR)/render.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	$(RM) $(TARGET) $(SDIR)/*.o

