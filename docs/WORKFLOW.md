# fiflo - workflow

## Remote repo (is upstream)
https://gitlab.com/mattmaniak/fiflo

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
- indentation style: 1 hard tab (4 spaces),
- use Allman-style like indentation,
- max. amount of '\n' in the row is: 2, eg. "}\n\n" - 1 seen "enter",
- single line can't exceed 80 characters.
- dont"t use camelCase (is unreadable), only lower-case snake_case is allowed,
- #defines have to be CAPITALIZED,
- keep names semantic shortened, eg. "fname_max",
- write short TODOs near thing to fix as uppercase comment,
- keep docs up-to-date with the code in stable releases,
- every function have to fit in the 720p screen with the 10px font,
- function"s args names need to be shorter than variables names.

## Testing
Before the stable release "fiflo" is tested with:
- LXTerminal,
- Terminology,
- Gnome Terminal,
- xterm
emulators
and with the native Linux text mode wnich is priority in tests.

I do my best for proper rendering in every above. In other cases editor can
works but there could be some glitches because of ther quirk modes.
