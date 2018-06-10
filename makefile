compile:
	gcc fiflo.c -o fiflo \
	-lasan \
	-fsanitize=address \
	-fsanitize=undefined \
	-fsanitize=signed-integer-overflow \
	-Wall \
	-Wextra \
	-Wpedantic

install:
	cp fiflo /usr/bin/fiflo

uninstall:
	rm /usr/bin/fiflo
