all: test compile
# In this case "all" means everything about test && compile.
# Installation and uninstallation are another cases.

test:
	./src/configure.sh

compile:
	gcc fiflo.c -o fiflo -lasan -fsanitize=address -fsanitize=signed-integer-overflow

install:
	cp fiflo /usr/bin/fiflo

uninstall:
	rm /usr/bin/fiflo
