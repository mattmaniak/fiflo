# fiflo

## Description
Simple terminal-based text editor.

## Requirements:
Linux-based OS with: gcc, make for compilation and optionally gzip to man-pages
installation. Any terminal that supports ANSI escape codes.

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
This will link AdressSanitizer to memory errors handling. Also you will be able
to run fiflo under gdb. You can also compile it normally and use Valgrind.

## FAQ
Will you port it into a Windows?
- No.

How can I contribute?
- Read the docs/WORKFLOW.md and send any request.
