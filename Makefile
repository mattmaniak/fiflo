CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic
ASAN_FLAGS = -lasan \
-fsanitize=address \
-fsanitize=undefined \
-fsanitize=signed-integer-overflow \

TARGET = fiflo
BIN_ODIR = /usr/bin/$(TARGET)
MAN_ODIR = /usr/share/man/man1/$(TARGET).1

all:
	$(CC) src/$(TARGET).c -o $(TARGET) \
	$(CFLAGS)

debug:
	$(CC) src/$(TARGET).c -o $(TARGET) \
	$(CFLAGS) \
	$(ASAN_FLAGS)

install:
	cp $(TARGET) $(BIN_ODIR)
	cp man/$(TARGET).1 $(MAN_ODIR)
	gzip $(MAN_ODIR)

uninstall:
	$(RM) $(BIN_ODIR)
	$(RM) $(MAN_ODIR).gz

clean:
	$(RM) $(TARGET)
