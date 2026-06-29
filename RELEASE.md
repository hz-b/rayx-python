# Release Workflow

This project uses a tag-driven release process.

- `pyproject.toml` keeps `dynamic = ["version"]`, so the release version comes from the git tag.
- `CHANGELOG.md` is the human-readable release record.
- GitHub Release notes are taken from the matching changelog section.

## Release Steps

Run the full interactive workflow:

```bash
bash tools/release.sh
```

Run a single step:

```bash
bash tools/release.sh --only <step>
```

Resume from a step:

```bash
bash tools/release.sh --start-at <step>
```

Supported steps:

- `bump`
- `commit`
- `tag`
- `push`
- `gh-release`
- `pr`
- `pypi`

## How the Changelog Works

Before a release, add release notes under `## [Unreleased]` in `CHANGELOG.md`.

During the `bump` step, the script:

- creates `CHANGELOG.md` if it does not exist
- requires `## [Unreleased]` to contain release entries
- creates `## [<version>] - <YYYY-MM-DD>` directly below `## [Unreleased]`
- moves unreleased entries into the new version section
- leaves `## [Unreleased]` empty for the next cycle

The resulting version section is also used as the notes file for `gh release create`.

## Restartability

The script stores the selected release version and notes path in `.git/release-state`, so later steps can be re-run without selecting the version again.

If a step has already been completed, the script will skip or reuse it when possible:

- existing local tags are reused
- existing remote tags stop tag creation to avoid conflicts
- existing GitHub releases are skipped
- existing PRs from the current branch to `main` are reused

## Notes

- The release tag must be in the form `v<version>`, for example `v0.5.0`.
- The script is written to work on GNU/Linux and macOS without relying on GNU-only `sed -i`.
