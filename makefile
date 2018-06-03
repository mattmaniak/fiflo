compile:
	gcc fiflo.c -o fiflo

install:
	cp fiflo /usr/bin/fiflo

uninstall:
	rm /usr/bin/fiflo
