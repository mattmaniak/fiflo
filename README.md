# fiflo

## Description
User-friendly ASCII terminal-based text editor.

## Requirements for the GNU/Linux:
- git,
- make,
- gcc or clang,
- gzip (only for an installation).

## Download, compile and run the current stable release
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

## (Un)install
```
sudo make install
```
```
sudo make uninstall
```

## Delete object files and the binary
```
make clean
```

## Link the AddressSanitizer for memory errors handling
```
make sanitize
```
The program will consume much more memory. Also it will be slower.

## FAQ
How can I contribute?
- Read the doc/WORKFLOW.md and send a request.

Will you port it into the Windows?
- No.
