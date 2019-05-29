# fiflo
Terminal-based text editor with Windows-like (CUA) bindings.

## Abstract
Learn C by writing pure C. It is exercise where I've started learning pointers
and memory management. As You can see it has grown to the text editor. It is
inspired by the Nano's minimalism and the Atom's rich features that make
programming easier.

## Usage
### Check the requirements for the GNU/Linux:
- git,
- make,
- gcc >= 4.9 or clang >= 3.6,
- gzip (only for the installation),
- sudo (only for the installation, even for the bare Debian on root).

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

### Optional cleanup - remove bin/ and obj/ after the compilation
```
make clean
```

### Uninstall, what You have installed
```
sudo make uninstall
```

## Development
Read the "CONTRIBUTING.md" and check the "doc/" directory.

### Source files (submodules):
- buffer - initialization and deleting the main buffer which holds the text
and it's metadata,semantic macros,

- chars - ascii codes that are interpreted as text,

- config - fiflorc parser and values setter,

- edit - more complex text editing operations that happens after the keypress,

- fiflo - the main file, just "main" and the execution loop,

- file - read/save the file, live editing the filename,

- input - gets the key and parses it,

- keys - keyboard bindings that don't insert chars, e.g. arrows,

- memory - (de)allocation of the memory for the text,

- options - "--help" and friends parameters,

- path - pathname, basename setting from arg,

- print - various magic that prints the text,

- ui - user interface components like bars.

- window - window rendering and flushing.

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
coverage. It will create the cov/ dir and put the every source file with codecov
marked after the previous execution The program must be compiled with the
"debug" option previously. Gcc only.
```
make coverage
```

Use the valgrind.
```
valgrind -v ./fiflo [optional args for the editor]
```
Remark: fiflo must be compiled without ASan and MSan.

### Install only the binary with ASan linked for debugging.
```
sudo make install_debug
```

## FAQ
Will you port it to the Windows?
- No.
