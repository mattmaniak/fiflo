# fiflo - workflow

## Remote repo (is upstream)
https://gitlab.com/mattmaniak/fiflo.git

## Branches
Everything happens on three branches:
- master,
- hotfix,
- develop.

## Branches description && merging
Everything connected with the development, tests etc. happens on the "develop"
branch. When everything is ok and the changelog is up-to-date content is merged
without fast-forwarding ("--no-ff" to make branches visualisation cleaner) to
the "stable". Then annotated tag is made on this branch. If there are any bugs,
typos... "stable" is "--no-ff" merged into "hotfix" and after repairs merged
back. Fixes from "hotfix" are made independently on the "develop" branch. But
sometimes "hotfix" can be merged back to "master" and then "develop" if a bug
will be discovered before changes on "develop". "Rebasing" isn"t used because
of destroying visualisations when used without parameters - merged is more
accurate for this purpose. OFC everything is pushed between one or a few
commits to provide updated code on the "develop" branch.

## Branching and versioning sources:
- https://semver.org/,
- https://nvie.com/posts/a-successful-git-branching-model/.

## Coding rules:
- Standard: C11 excluding "pragma pack",
- use Allman's brace style, indentation: hard tab (4 spaces wide),
- max. amount of '\n' in the row is: 2, eg. "}\n\n" - 1 seen "enter",
- single line can't exceed 80 characters, function - 40 lines,
- funcion names shouldn't contain underscore, variables should,
- keep names semantic shortened, eg. "fname_max",
- write short TODOs near thing to fix as uppercase comments,
- keep docs up-to-date with the code in stable releases,
- function"s args names need to be shorter than variables' names.

## Testing
Before the stable release, fiflo is tested with the native Linux tty. 
Correct rendering of the window depends on terminal emulator's implementations.
