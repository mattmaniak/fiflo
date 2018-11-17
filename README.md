# fiflo

## Description
ASCII terminal-based text editor.

## Requirements for the GNU/Linux:
- git,
- make,
- clang or gcc,
- gzip (only for the installation).

## Download, compile and run current stable release
```
git clone https://gitlab.com/mattmaniak/fiflo.git
cd fiflo
make
./fiflo
```

Also You can try the latest unstable snapshot
```
git clone https://gitlab.com/mattmaniak/fiflo.git -b develop
```

## Install
```
sudo make install
```
After that, there is a possibility to run it directly.
```
fiflo
```

## Uninstall
```
sudo make uninstall
```

## Delete object files and the binary.
```
make clean
```

## Link AddressSanitizer
```
make address
```
Warning! The program will consume much more memory. Also it will be slower.

## Link MemorySanitizer (only with clang)
```
make memory
```
Causes 1.5-2.5x slowdown.

## FAQ
How can I contribute?
- Read the doc/WORKFLOW.md and send a request.

Will you port it into the Windows?
- No.

