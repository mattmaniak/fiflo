# fiflo

## Description
Simple terminal-based text editor.

## Needed tools for building and installation:
- gcc,
- gzip,
- make.

## Compilation
```
make
```

## Installation
```
sudo make install
```

## Uninstallation
```
sudo make uninstall
```

## Testing
```
make debug
```
This will link AdressSanitizer to better memory errors handling. Also you will
be able to run fiflo under gdb. You can also compile it normally and use
Valgrind.

## FAQ
Will you port it into a Windows?
- No.

How can I contribute?
- Read the docs/WORKFLOW.md and send any request.
