# fiflo - contributing

When contributing to this repository, please first discuss the change you wish
to make via issue, email, or any other method with the owners of this repo
before making a change.

## Workflow
### Branching
[V. Driessen's model](https://nvie.com/posts/a-successful-git-branching-model/)
As described but only with: master, develop, hotifx branches.

### Version scheme
[SemVer](https://semver.org/)

### Coding rules:
- Allman-like brace style,
- indentation: 1 hard tab (width: 4),
- only snake_case is allowed,
- visually align: defines, types, names, assignments etc,
- single line should not exceed 80 chars printable chars,
- e,g function "f_name" in the file asdf.c should be named "asdf__f_name","
- insert short TODOs near thing to fix as uppercase comments,
- write documentation comments,
- keep docs up-to-date with the code in stable releases,

## Merge request
- As the source base and it's develop branch can be found on
[GitLab](https://gitlab.com/mattmaniak/fiflo/tree/develop) it would great to
Use that platform by You to improve the code.

- Before requesting any change, check that your code matches basic standards
described above and test it using the default Linux tty. Also You can open an
issue instead of a request.

- Note that the maintainer is responsible for merging, tagging, generally
versioning.