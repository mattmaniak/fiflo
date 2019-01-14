# fiflo - nano-inspired text editor with CUA keyboard shortcuts

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

### Object files and the binary removal
```
make clean
```

### Uninstallation
```
sudo make uninstall
```

## Development
Dive into the "doc/" direcory.

### Additional tools:
- MSan (clang >= 3.1),
- valgrind.

### Get and check the development branch
```
git clone https://gitlab.com/mattmaniak/fiflo.git -b develop
git checkout develop
```

### Debugging
Link the AddressSanitizer. Causes slowdown and huge memory usage.
```
make address
```

Link the MemorySanitizer (only with clang). Causes slowdown.
```
make memory
```

Use the valgrind.
```
valgrind -v ./fiflo [optional args]
```
Note: fiflo must be compiled without ASan and MSan.

## FAQ
Will you port it into the Windows?
- No.
