# fiflo
Terminal-based text editor with Common User Access keyboard shortcuts.

## Usage
### Check the requirements for the GNU/Linux:
- git,
- make,
- gcc >= 4.9 or clang >= 3.6,
- gzip (only for the installation),
- sudo (only for the installation).

### Clone on a desktop, compile and run.
```
git clone https://gitlab.com/mattmaniak/fiflo.git &&
cd fiflo &&
make &&
bin/fiflo
```

### Install
```
make install
```
Writing "sudo" before it will break the config file permissions.

Then use it directly from a disk...
```
fiflo
```

or better read the  manual...
```
man fiflo
```

and it's configuration guide.
```
man fiflorc
```

### Optional cleanup - remove "bin"/ and "obj/" after the compilation
```
make clean
```

### Uninstall, what You have installed
```
sudo make uninstall
```

## Development
Read the "CONTRIBUTING.md" and check the "doc/" directory.

### Source files:
- buffer.c - initialization and deleting the main buffer which holds the text
and it's metadata,semantic macros,

- config.c - fiflorc parser and values setter,

- edit.c - more complex text editing operations that happens after the keypress,

- fiflo.c - the main file, just "main" and the execution loop,

- file.c - read/save the file, live editing the filename,

- input.c - get the key from the keypress, parse it,

- keys.c - basic actions of every supported key,

- memory.c - allocation/deallocation of the memory for the text,

- options.c - "--help" and friends parameters,

- print.c - various magic that prints the text,

- ui.c - user interface components like bars.

- window.c - window rendering and flushing.

### Needed tools for debugging:
- AddressSanitizer (built-in gcc and clang),
- gcov,
- valgrind.

### Get and set the development branch
```
git clone https://gitlab.com/mattmaniak/fiflo.git -b develop &&
git checkout develop
```

### Debugging
Link the AddressSanitizer and add support fot the gcov. Causes slowdown and huge
memory usage.
```
make debug
```

After that and the fiflo execution, there is possibility to check the code
coverage. It will create the "cov/" dir and put the every source file with
codecov marked after the previous execution The program must be compiled with
the "debug" option previously.
```
make coverage
```

Use the valgrind.
```
valgrind -v ./fiflo [optional args]
```
Remark: fiflo must be compiled without ASan and MSan.

### Install only the debugable binary.
```
sudo make debug_install
```

## FAQ
Will you port it into the Windows?
- No.
