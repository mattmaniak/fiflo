# fiflo - source files description
Every .c file and it's role.

## Files:
- buffer.c - initialization and deleting the main buffer which holds the text
and it's metadata,

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

- window.c - rendering the window.
