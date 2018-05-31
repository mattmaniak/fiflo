# Fiflo - workflow

## Branches
Everything happens on three branches:
- stable (main branch),
- develop,
- hotfix.

## Description
Everything connected with the development, tests etc. happens on the "develop"
branch. When everything is ok and the changelog is up-to-date content is merged
without fast-forwarding ("--no-ff" to make branches visualisation cleaner) to
the "stable". Then annotated tag is made on this branch. If there are any bugs,
typos... "stable" is "--no-ff" merged into "hotfix" and after repairs merged
back. Fixes from "hotfix" are made independently on the "develop" branch.
"Rebasing" isn't used because of destroying visualisations when used without
parameters - merged is more accurate for this purpose. OFC everything is pushed
between one or a few commits to provide updated code on the "develop" branch.

## Sources I used:
- https://semver.org/,
- https://nvie.com/posts/a-successful-git-branching-model/ and between,
- https://guides.github.com/introduction/flow/.

