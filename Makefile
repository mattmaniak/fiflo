CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic

DEBUG_FLAGS = -g -lasan \
-fsanitize=address \
-fsanitize=undefined \
-fsanitize=signed-integer-overflow \

compile:
	$(CC) src/fiflo.c -o fiflo \
	$(CFLAGS) \
	$(DEBUG_FLAGS)

test: # ONLY FOR THE WIP
	$(CC) src/fiflo.c -o fiflo \
	$(CFLAGS)

	valgrind \
	--tool=memcheck \
	--leak-check=yes \
	--show-reachable=yes \
	--num-callers=20 \
	--track-fds=yes \
	./fiflo

install:
	cp fiflo /usr/bin/fiflo
	cp man/fiflo.1 /usr/share/man/man1/fiflo.1
	gzip /usr/share/man/man1/fiflo.1

uninstall:
	rm /usr/bin/fiflo
	rm /usr/share/man/man1/fiflo.1.gz
