# fiflo
Terminal-based text editor with Common User Access keyboard shortcuts.

## Use
### Check the requirements for the GNU/Linux:
- git,
- make,
- gcc >= 4.6 or clang >= 3.1,
- gzip (only for the installation),
- sudo (e.g. if You're running the bare Debian on root...)

### Clone on a desktop, compile and run.
```
git clone https://gitlab.com/mattmaniak/fiflo.git &&
cd fiflo &&
make &&
bin/fiflo
```

### Install
```
sudo make install
```

Then use it directly from a disk...
```
./fiflo
```

or better read the  manual...
```
man fiflo
```

and it's configuration guide.
```
man fiflorc
```

### Optional cleanup - remove "bin" and "obj" directories after the compilation.
```
make clean
```

### Uninstall, what You have installed.
```
sudo make uninstall
```

## Development
Dive into the "doc/" direcory.

### Recommended tools for debugging:
- AddressSanitizer (included in gcc and clang),
- MemorySanitizer (clang >= 3.1 only),
- valgrind.

### Get and set the development branch
```
git clone https://gitlab.com/mattmaniak/fiflo.git -b develop &&
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

### Install only the binary with ASan linked
```
sudo make install_debug
```

## FAQ
Will you port it into the Windows?
- No.
