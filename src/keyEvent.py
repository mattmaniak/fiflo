#! /usr/python3

import sys
import termios
import tty

# https://code.activestate.com/recipes/134892/
fd = sys.stdin.fileno()
oldSettings = termios.tcgetattr(fd)
try:
	tty.setraw(sys.stdin.fileno())
	pressedKey = sys.stdin.read(1)

finally:
	termios.tcsetattr(fd, termios.TCSADRAIN, oldSettings)

termios.tcflush(sys.stdin, termios.TCIOFLUSH)

