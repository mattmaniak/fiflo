# Fiflo
## Git workflow
Everything happens on three branches:
- stable (main branch),
- develop,
- hotfix.
Everything connected with the development, tests etc. happens on the "develop"
branch. When everything is ok and the changelog is up-to-date content is merged
without fast-forwarding ("--no-ff" to make branches visualisation cleaner) to
the "stable". Then annotated tag is made on this branch. If there are any bugs,
typos... "stable" is "--no-ff" merged into "hotfix" and after repairs merged
back. Fixes from "hotfix" are made independently on the "develop" branch.
"Rebasing" isn't used because of destroying visualisations when used without
parameters - merged is more accurate for this purpose.

