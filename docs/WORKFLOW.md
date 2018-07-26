# fiflo - workflow

## Remote repo
https://gitlab.com/mattmaniak/fiflo

## Branches
Everything happens on three branches:
- stable (main branch),
- develop,
- hotfix.

## Branches description
Everything connected with the development, tests etc. happens on the "develop"
branch. When everything is ok and the changelog is up-to-date content is merged
without fast-forwarding ("--no-ff" to make branches visualisation cleaner) to
the "stable". Then annotated tag is made on this branch. If there are any bugs,
typos... "stable" is "--no-ff" merged into "hotfix" and after repairs merged
back. Fixes from "hotfix" are made independently on the "develop" branch. But
sometimes 'hotfix' can be merged back to 'master' and then 'develop' if a bug
will be discovered before changes on 'develop'. "Rebasing" isn't used because
of destroying visualisations when used without parameters - merged is more
accurate for this purpose. OFC everything is pushed between one or a few
commits to provide updated code on the "develop" branch.

## Goog sources:
- https://semver.org/,
- https://nvie.com/posts/a-successful-git-branching-model/.

## Coding rules:
- use Allman-style indentation,
- comment a lot,
- write short TODOs near thing to fix as uppercase comment,
- name longer variables with an underscore, functions with camelCase,
- #define as much as you can,
- keep docs up-to-date with your code in stable releases,
- function's args names should be shorter than variables names,
- no "typedef struct".
