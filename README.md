# fiflo

## Description
Simple terminal-based text editor.

## Minimal requirements for compilation:
- Linux kernel,
- autoconf,
- make,
- gcc.

## Additional requirements for man-page installation and VCS:
- gzip,
- git.

## Downloading
Current stable release:
```
git clone https://gitlab.com/mattmaniak/fiflo.git
```
Latest unstable snapshot:
```
git clone https://gitlab.com/mattmaniak/fiflo.git -b develop
```

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

## Building with the AddressSanitizer for memory errors handling
```
make sanitize
```

## FAQ
How can I contribute?
- Read the doc/WORKFLOW.md and send request.

Will you port it into the Windows?
- No.

