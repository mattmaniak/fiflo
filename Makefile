compile:
	gcc src/fiflo.c -o fiflo \
	-lasan \
	-fsanitize=address \
	-fsanitize=undefined \
	-fsanitize=signed-integer-overflow \
	-Wall \
	-Wextra \
	-Wpedantic \
	-g

install:
	cp fiflo /usr/bin/fiflo
	cp man/fiflo.1 /usr/share/man/man1/fiflo.1
	gzip /usr/share/man/man1/fiflo.1

uninstall:
	rm /usr/bin/fiflo
	rm /usr/share/man/man1/fiflo.1.gz
