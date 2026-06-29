#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${PROJECT_ROOT}"

CHANGELOG_FILE="CHANGELOG.md"
STATE_FILE="$(git rev-parse --git-path release-state)"
NOTES_FILE="$(git rev-parse --git-path release-notes.md)"
STEPS=("bump" "commit" "tag" "push" "gh-release" "pr" "pypi")

SELECTED_VERSION=""
SELECTED_TAG=""
RELEASE_DATE=""
RUN_MODE="default"
TARGET_STEP=""

usage() {
    cat <<'EOF'
Usage: bash tools/release.sh [--only <step> | --start-at <step>]

Steps:
  bump        Select next version and update CHANGELOG.md
  commit      Commit release changelog changes
  tag         Create git tag v<version>
  push        Push commit and tag to origin
  gh-release  Create GitHub release from CHANGELOG.md notes
  pr          Create GitHub PR to main from current branch
  pypi        Show PyPI publishing reminder
EOF
}

error() {
    echo "Error: $*" >&2
    exit 1
}

print_header() {
    echo
    echo "============================================================"
    echo "$1"
    echo "============================================================"
}

confirm() {
    local prompt="$1"
    local reply

    read -rp "${prompt} [y/N]: " reply
    [[ "${reply}" == "y" || "${reply}" == "Y" ]]
}

require_command() {
    if ! command -v "$1" >/dev/null 2>&1; then
        error "Required command not found: $1"
    fi
}

step_exists() {
    local needle="$1"
    local step

    for step in "${STEPS[@]}"; do
        if [[ "${step}" == "${needle}" ]]; then
            return 0
        fi
    done

    return 1
}

step_index() {
    local needle="$1"
    local i

    for i in "${!STEPS[@]}"; do
        if [[ "${STEPS[i]}" == "${needle}" ]]; then
            echo "${i}"
            return 0
        fi
    done

    return 1
}

parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --only)
                [[ $# -ge 2 ]] || error "--only requires a step name"
                [[ "${RUN_MODE}" == "default" ]] || error "--only and --start-at cannot be combined"
                RUN_MODE="only"
                TARGET_STEP="$2"
                shift 2
                ;;
            --start-at)
                [[ $# -ge 2 ]] || error "--start-at requires a step name"
                [[ "${RUN_MODE}" == "default" ]] || error "--only and --start-at cannot be combined"
                RUN_MODE="start-at"
                TARGET_STEP="$2"
                shift 2
                ;;
            --help|-h)
                usage
                exit 0
                ;;
            *)
                error "Unknown option: $1"
                ;;
        esac
    done

    if [[ "${RUN_MODE}" != "default" ]] && ! step_exists "${TARGET_STEP}"; then
        error "Unknown step: ${TARGET_STEP}"
    fi
}

step_should_run() {
    local step="$1"

    case "${RUN_MODE}" in
        default)
            return 0
            ;;
        only)
            [[ "${step}" == "${TARGET_STEP}" ]]
            ;;
        start-at)
            [[ "$(step_index "${step}")" -ge "$(step_index "${TARGET_STEP}")" ]]
            ;;
        *)
            return 1
            ;;
    esac
}

create_temp_file() {
    mktemp "${TMPDIR:-/tmp}/rayx-release.XXXXXX"
}

save_state() {
    local tmp_file

    tmp_file="$(create_temp_file)"
    {
        printf 'SELECTED_VERSION=%q\n' "${SELECTED_VERSION}"
        printf 'SELECTED_TAG=%q\n' "${SELECTED_TAG}"
        printf 'RELEASE_DATE=%q\n' "${RELEASE_DATE}"
        printf 'NOTES_FILE=%q\n' "${NOTES_FILE}"
    } > "${tmp_file}"
    mv "${tmp_file}" "${STATE_FILE}"
}

load_state() {
    if [[ -f "${STATE_FILE}" ]]; then
        # shellcheck disable=SC1090
        source "${STATE_FILE}"
    fi
}

require_state() {
    load_state

    [[ -n "${SELECTED_VERSION}" ]] || error "No release version selected yet. Run the 'bump' step first."
    [[ -n "${SELECTED_TAG}" ]] || SELECTED_TAG="v${SELECTED_VERSION}"
    [[ -n "${RELEASE_DATE}" ]] || RELEASE_DATE="$(date +%F)"
}

ensure_changelog_exists() {
    if [[ -f "${CHANGELOG_FILE}" ]]; then
        return
    fi

    local tmp_file
    tmp_file="$(create_temp_file)"
    cat <<'EOF' > "${tmp_file}"
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
EOF
    mv "${tmp_file}" "${CHANGELOG_FILE}"
}

current_release_tag() {
    git describe --tags --abbrev=0 --match 'v[0-9]*.[0-9]*.[0-9]*' 2>/dev/null || echo "v0.0.0"
}

current_release_version() {
    current_release_tag | sed 's/^v//'
}

validate_version() {
    [[ "$1" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || error "Version must use semantic version format: X.Y.Z"
}

choose_version() {
    local current_version major minor patch version_choice

    current_version="$(current_release_version)"
    IFS='.' read -r major minor patch <<< "${current_version}"

    print_header "Current version: ${current_version}"
    echo
    echo "Select version bump:"
    echo "1) patch (${major}.${minor}.$((patch + 1)))"
    echo "2) minor (${major}.$((minor + 1)).0)"
    echo "3) major ($((major + 1)).0.0)"
    echo "4) custom"

    read -rp "Choice [1-4]: " version_choice

    case "${version_choice}" in
        1)
            SELECTED_VERSION="${major}.${minor}.$((patch + 1))"
            ;;
        2)
            SELECTED_VERSION="${major}.$((minor + 1)).0"
            ;;
        3)
            SELECTED_VERSION="$((major + 1)).0.0"
            ;;
        4)
            read -rp "Enter version: " SELECTED_VERSION
            ;;
        *)
            error "Invalid choice."
            ;;
    esac

    validate_version "${SELECTED_VERSION}"
    SELECTED_TAG="v${SELECTED_VERSION}"
    RELEASE_DATE="$(date +%F)"

    echo
    echo "New version: ${SELECTED_VERSION}"

    if ! confirm "Proceed?"; then
        echo "Aborted."
        exit 0
    fi
}

extract_section_body() {
    local heading="$1"

    awk -v heading="${heading}" '
        $0 == heading { in_section = 1; next }
        in_section && /^## \[/ { exit }
        in_section { print }
    ' "${CHANGELOG_FILE}"
}

section_exists() {
    grep -Fqx "## [${SELECTED_VERSION}] - ${RELEASE_DATE}" "${CHANGELOG_FILE}"
}

section_has_release_entries() {
    local heading="$1"

    extract_section_body "${heading}" | awk '
        /^[[:space:]]*$/ { next }
        /^### [[:alnum:]][[:print:]]*$/ { next }
        { found = 1; exit }
        END { exit found ? 0 : 1 }
    '
}

write_release_notes() {
    local heading="## [${SELECTED_VERSION}] - ${RELEASE_DATE}"
    local tmp_file

    tmp_file="$(create_temp_file)"
    extract_section_body "${heading}" | sed '/^[[:space:]]*$/d' > "${tmp_file}"

    if [[ ! -s "${tmp_file}" ]]; then
        rm -f "${tmp_file}"
        error "Could not extract release notes for ${SELECTED_TAG} from ${CHANGELOG_FILE}"
    fi

    mv "${tmp_file}" "${NOTES_FILE}"
}

update_changelog_for_release() {
    local release_heading="## [${SELECTED_VERSION}] - ${RELEASE_DATE}"
    local unreleased_body tmp_file

    ensure_changelog_exists

    grep -Fqx '## [Unreleased]' "${CHANGELOG_FILE}" || error "CHANGELOG.md is missing the '## [Unreleased]' section."

    if section_exists; then
        if section_has_release_entries '## [Unreleased]'; then
            error "CHANGELOG.md already contains ${release_heading}, but [Unreleased] still has entries."
        fi

        write_release_notes
        save_state
        return
    fi

    unreleased_body="$(extract_section_body '## [Unreleased]')"
    if ! printf '%s\n' "${unreleased_body}" | awk '
        /^[[:space:]]*$/ { next }
        /^### [[:alnum:]][[:print:]]*$/ { next }
        { found = 1; exit }
        END { exit found ? 0 : 1 }
    '; then
        error "The [Unreleased] section in CHANGELOG.md has no release entries."
    fi

    tmp_file="$(create_temp_file)"
    awk -v release_heading="${release_heading}" '
        $0 == "## [Unreleased]" {
            print
            print ""
            print release_heading
            print ""
            in_unreleased = 1
            next
        }

        in_unreleased && /^## \[/ {
            print ""
            in_unreleased = 0
        }

        in_unreleased {
            print
            next
        }

        {
            print
        }
    ' "${CHANGELOG_FILE}" > "${tmp_file}"
    mv "${tmp_file}" "${CHANGELOG_FILE}"

    write_release_notes
    save_state
}

remote_tag_exists() {
    git ls-remote --exit-code --tags origin "refs/tags/${SELECTED_TAG}" >/dev/null 2>&1
}

local_tag_exists() {
    git rev-parse -q --verify "refs/tags/${SELECTED_TAG}" >/dev/null 2>&1
}

gh_cli_instructions() {
    echo "GitHub CLI (gh) is not installed."
    echo
    echo "Install it from: https://cli.github.com/"
    echo
    echo "Then authenticate with:"
    echo
    echo "  gh auth login"
}

run_bump_step() {
    print_header "Preparing release changelog"
    choose_version
    update_changelog_for_release

    echo
    echo "Prepared ${CHANGELOG_FILE} for ${SELECTED_TAG}."
    git status --short
}

run_commit_step() {
    print_header "Committing release changes"
    require_state

    if ! confirm "Create git commit for release changes?"; then
        echo "Skipping git commit."
        return
    fi

    git add "${CHANGELOG_FILE}"

    if git diff --cached --quiet; then
        echo "No staged changelog changes detected."
        echo "Skipping git commit."
        return
    fi

    git commit -m "Release ${SELECTED_TAG}"
}

run_tag_step() {
    print_header "Creating git tag"
    require_state

    if ! confirm "Create git tag ${SELECTED_TAG}?"; then
        echo "Skipping git tag creation."
        return
    fi

    if remote_tag_exists; then
        error "Git tag ${SELECTED_TAG} already exists on origin. Aborting to avoid conflicting releases."
    fi

    if local_tag_exists; then
        echo "Git tag ${SELECTED_TAG} already exists locally."
        return
    fi

    git tag "${SELECTED_TAG}"
    echo "Created git tag: ${SELECTED_TAG}"
}

run_push_step() {
    local current_branch

    print_header "Pushing release"
    require_state

    if ! confirm "Push commit and tag to origin?"; then
        echo "Skipping push."
        return
    fi

    current_branch="$(git branch --show-current)"
    [[ -n "${current_branch}" ]] || error "Could not determine the current branch."

    git push origin "${current_branch}"

    if remote_tag_exists; then
        echo "Git tag ${SELECTED_TAG} already exists on origin."
    else
        git push origin "${SELECTED_TAG}"
    fi

    echo "Pushed branch and tag."
}

run_gh_release_step() {
    print_header "Checking GitHub CLI"
    require_state

    if ! command -v gh >/dev/null 2>&1; then
        echo
        gh_cli_instructions
        return
    fi

    if ! confirm "Create GitHub release ${SELECTED_TAG}?"; then
        echo "Skipping GitHub release creation."
        return
    fi

    if gh release view "${SELECTED_TAG}" >/dev/null 2>&1; then
        echo "GitHub release ${SELECTED_TAG} already exists."
        return
    fi

    write_release_notes

    gh release create \
        "${SELECTED_TAG}" \
        --title "${SELECTED_TAG}" \
        --notes-file "${NOTES_FILE}"
}

find_existing_pr_number() {
    local current_branch="$1"

    gh pr list \
        --head "${current_branch}" \
        --base main \
        --json number \
        --limit 1 \
        --jq '.[0].number // empty'
}

run_pr_step() {
    local current_branch pr_number pr_title pr_body merge_choice

    print_header "Creating GitHub PR"
    require_state

    if ! command -v gh >/dev/null 2>&1; then
        echo
        gh_cli_instructions
        return
    fi

    current_branch="$(git branch --show-current)"
    if [[ "${current_branch}" == "main" ]]; then
        echo "Current branch is main; skipping PR creation."
        return
    fi

    if ! confirm "Create GitHub PR to main from ${current_branch}?"; then
        echo "Skipping PR creation."
        return
    fi

    pr_number="$(find_existing_pr_number "${current_branch}")"
    if [[ -z "${pr_number}" ]]; then
        pr_title="Release ${SELECTED_TAG}: ${current_branch} -> main"
        pr_body=$(
            cat <<EOF
## Release promotion

- Promote release changes from \`${current_branch}\` to \`main\`.
- Version: \`${SELECTED_TAG}\`
EOF
        )

        gh pr create \
            --base main \
            --head "${current_branch}" \
            --title "${pr_title}" \
            --body "${pr_body}"

        pr_number="$(find_existing_pr_number "${current_branch}")"
    else
        echo "GitHub PR #${pr_number} already exists for ${current_branch} -> main."
    fi

    [[ -n "${pr_number}" ]] || return

    echo
    echo "Select PR merge mode:"
    echo "1) do not merge automatically"
    echo "2) merge now (if allowed)"
    echo "3) enable auto-merge when checks pass"
    read -rp "Choice [1-3]: " merge_choice

    case "${merge_choice}" in
        2)
            if [[ "${current_branch}" == "develop" ]]; then
                gh pr merge "${pr_number}" --merge
            else
                gh pr merge "${pr_number}" --merge --delete-branch
            fi
            ;;
        3)
            if [[ "${current_branch}" == "develop" ]]; then
                gh pr merge "${pr_number}" --auto --merge
            else
                gh pr merge "${pr_number}" --auto --merge --delete-branch
            fi
            ;;
        *)
            echo "Skipping automatic PR merge."
            ;;
    esac
}

run_pypi_step() {
    print_header "PyPI publishing"

    if confirm "Upload package to PyPI?"; then
        echo "Note: PyPI publish script not yet created. Run publish manually or add tools/publish_pypi.sh."
    fi
}

main() {
    local step

    parse_args "$@"
    require_command git

    for step in "${STEPS[@]}"; do
        if ! step_should_run "${step}"; then
            continue
        fi

        case "${step}" in
            bump)
                run_bump_step
                ;;
            commit)
                run_commit_step
                ;;
            tag)
                run_tag_step
                ;;
            push)
                run_push_step
                ;;
            gh-release)
                run_gh_release_step
                ;;
            pr)
                run_pr_step
                ;;
            pypi)
                run_pypi_step
                ;;
        esac
    done

    print_header "Release workflow completed"
}

main "$@"
