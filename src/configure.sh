#! /bin/bash

function checkHeader {
	find /usr/include/$1
}

type gcc
checkHeader stdio.h
checkHeader stdint.h
checkHeader stdlib.h
checkHeader string.h
checkHeader sys/ioctl.h # TODO: doesn't found!
checkHeader unistd.h
checkHeader termios.h

echo "Ignore if you haven't got 'sys/ioctl.h' but the program has compiled."

