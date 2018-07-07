CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic

ASAN_FLAGS = -g -lasan \
-fsanitize=address \
-fsanitize=undefined \
-fsanitize=signed-integer-overflow \

VALGRIND_FLAGS = --tool=memcheck \
	--leak-check=yes \
	--show-reachable=yes \
	--num-callers=20 \
	--track-fds=yes \

TARGET = fiflo
INSTALL_ODIR = /usr/bin/$(TARGET)
MAN_ODIR = /usr/share/man/man1/$(TARGET).1

all:
	$(CC) src/$(TARGET).c -o $(TARGET) \
	$(CFLAGS) \
	$(ASAN_FLAGS)

test: # ONLY FOR THE WIP
	$(CC) src/$(TARGET).c -o $(TARGET) \
	$(CFLAGS)

	valgrind \
	$(VALGRIND_FLAGS) \
	./$(TARGET)

install:
	cp $(TARGET) $(INSTALL_DIR)
	cp man/$(TARGET).1 $(MAN_ODIR)
	gzip $(MAN_ODIR)

uninstall:
	$(RM) $(INSTALL_ODIR)
	$(RM) $(MAN_ODIR).gz

clean:
	$(RM) $(TARGET)
