# fiflo - workflow

## Remote repo
https://gitlab.com/mattmaniak/fiflo.git (upstreamed)

## Branches:
- master,
- hotfix,
- develop.
Inspiration: https://nvie.com/posts/a-successful-git-branching-model/.

Everything connected with the development, tests etc. happens on the "develop".
When everything is ok and the changelog is up-to-date content is merged without
fast-forwarding ("--no-ff" to make branches visualisation cleaner) to the
"stable". Then annotated tag is made. If there are any bugs, "stable" is
"--no-ff" merged into "hotfix" and after repairs merged back. Fixes from
"hotfix" are made independently on the "develop". But sometimes "hotfix" can be
merged back to "develop" through "master" if a bug will was discovered before
any development commit. Everything is pushed between one or a few commits to
provide updated code on the "develop" branch.

## Coding rules:
- Allman-like brace style, indentation: hard tab (equal to 4 spaces),
- only snake_case is allowed,
- use visual alignment in declarations and definitons, it's helpful,
- single line should not exceed 80 chars printable chars,
- insert short TODOs near thing to fix as uppercase comments,
- write doc comments above declarations, statements etc.,
- keep docs up-to-date with the code in stable releases,

## Versions
Based on the https://semver.org.

## Testing
Daily with the LXTerminal. Before the stable release, fiflo is tested with the
native Linux tty. Correct rendering of the window can depend on quirk emulators
implementations.
