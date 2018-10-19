# fiflo

## Description
Simple terminal-based text editor.

## Requirements for:
### 1. Compilation:
- Linux-based desktop,
- make,
- gcc or clang.

### 2. Installation:
- gzip.

### 3. Developent and testing:
- git,
- valgrind.

## Downloading
### Using git:
Current stable release:
```
git clone https://gitlab.com/mattmaniak/fiflo.git
```
Latest unstable snapshot:
```
git clone https://gitlab.com/mattmaniak/fiflo.git -b develop
```

### Or just click "Download" on the GitLab's repository page.

## Building from source
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

## Memory debugging with an ASan.
```
make sanitize
```

## Delete object files and the binary
```
make clean
```

## FAQ
How can I contribute?
- Read the doc/WORKFLOW.md and send request.

Will you port it into the Windows?
- No.

