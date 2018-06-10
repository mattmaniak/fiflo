compile:
	gcc fiflo.c -o fiflo \
	-Wall \
	-Wextra \
	-Wpedantic

install:
	cp fiflo /usr/bin/fiflo

uninstall:
	rm /usr/bin/fiflo
