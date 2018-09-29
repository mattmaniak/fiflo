# fiflo

## Description
Simple terminal-based text editor.

## Minimal requirements:
- Linux-based OS,
- autoconf,
- make,
- gcc,
- terminal with an ANSI escape codes support.

# Additional requirements:
- gzip for man-pages packing,
- cp for installation.

## Building from source
```
autoconf && ./configure && make
```

## Installation
```
sudo make install
```

## Uninstallation
```
sudo make uninstall
```

## Building with AddressSanitizer
```
make sanitize
```

## FAQ
Will you port it into the Windows?
- No.

How can I contribute?
- Read the doc/WORKFLOW.md and send any request.
