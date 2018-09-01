# fiflo

## Description
Simple terminal-based text editor.

## Minimal requirements:
- Linux-based OS,
- 64-bit gcc (min. tested 4.8.4),
- make,
- terminal with an ANSI escape codes support.

# Additional requirements:
- cp for installation,
- gzip for man-pages packing.

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
This will link AdressSanitizer to memory errors handling. You will be able to
run fiflo under gdb. I prefer to compile it normally and use Valgrind.

## FAQ
Will you port it into the Windows?
- No.

How can I contribute?
- Read the docs/WORKFLOW.md and send any request.
