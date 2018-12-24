# fiflo

## Description
User-friendly ASCII terminal-based text editor.

## Get it
### Requirements for the GNU/Linux:
- git,
- make,
- gcc >= 4.6 or clang >= 3.1,
- gzip (only for an installation).

### Quick guide to run it
```
git clone https://gitlab.com/mattmaniak/fiflo.git
cd fiflo
make
bin/fiflo
```

Also You can try the latest unstable snapshot
```
git clone https://gitlab.com/mattmaniak/fiflo.git -b develop
```

### Installation
```
sudo make install
```
Then it will be posibility to run it directly from disk.
```
./fiflo
```

### Uninstallation
```
sudo make uninstall
```

### Delete object files and the binary
```
make clean
```

## Development
Dive into the "doc/" direcory.

### Additional tools:
- clang >= 3.1,
- valgrind.

### Get and check the development branch
```
git clone https://gitlab.com/mattmaniak/fiflo.git -b develop
git checkout develop
```

### Debugging
Link the AddressSanitizer. Causes slowdown and huge memory usage.
```
make asan
```

Link the MemorySanitizer (only with clang). Causes slowdown.
```
make msan
```

Use the valgrind.
```
make valgrind FIFLO_ARG=<parameter passed to the fiflo>
```
Note: FIFLO_ARG is optional.

## FAQ
Will you port it into the Windows?
- No.
