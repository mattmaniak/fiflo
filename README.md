# fiflo

## Description
ASCII terminal-based text editor.

## Requirements:
- Linux-based desktop,
- git,
- make,
- gcc or clang,
- gzip (only for installation).

## Download, compile and run current stable release
```
git clone https://gitlab.com/mattmaniak/fiflo.git
make
./fiflo
```

Also you can use the latest unstable snapshot
```
git clone https://gitlab.com/mattmaniak/fiflo.git -b develop
```

## Install
```
sudo make install
```

## Uninstall
```
sudo make uninstall
```

## Delete object files and the binary
```
make clean
```

## Test using AddressSanitizer
```
make memory
```

## FAQ
How can I contribute?
- Read the doc/WORKFLOW.md and send a request.

Will you port it into the Windows?
- No.

