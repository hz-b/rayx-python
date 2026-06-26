#!/usr/bin/env bash

set -euo pipefail

LAUNCH_HTML=false
BUILD_HTML=false
EXPLICIT_BUILD_SELECTION=false

for arg in "$@"; do
    case "$arg" in
        --html)
            BUILD_HTML=true
            EXPLICIT_BUILD_SELECTION=true
            ;;
        --open)
            LAUNCH_HTML=true
            ;;
        *)
            echo "Unknown option: $arg"
            echo "Usage: $0 [--html] [--open]"
            exit 1
            ;;
    esac
done

if [[ "${EXPLICIT_BUILD_SELECTION}" == false ]]; then
    BUILD_HTML=true
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
DOCS_DIR="${PROJECT_ROOT}/docs"
DOCS_VENV_DIR="${PROJECT_ROOT}/venv_docs"
UV_BIN="${UV_BIN:-uv}"

if ! command -v "${UV_BIN}" >/dev/null 2>&1; then
    echo "Error: 'uv' is required to bootstrap docs environment."
    echo "Install uv first: https://docs.astral.sh/uv/getting-started/installation/"
    exit 1
fi

if [[ ! -x "${DOCS_VENV_DIR}/bin/python" ]]; then
    echo "============================================================"
    echo "Creating docs virtual environment: ${DOCS_VENV_DIR}"
    echo "============================================================"
    "${UV_BIN}" venv --python 3.12 "${DOCS_VENV_DIR}"
fi

echo "============================================================"
echo "Installing docs dependencies into ${DOCS_VENV_DIR}"
echo "============================================================"
DOCS_DEPS="$(python3 -c "import tomllib; d=tomllib.loads(open('${PROJECT_ROOT}/pyproject.toml','rb').read()); print(' '.join(d['project']['optional-dependencies']['docs']))")"
"${UV_BIN}" pip install --python "${DOCS_VENV_DIR}/bin/python" rayx ${DOCS_DEPS}

PYTHON_BIN="${DOCS_VENV_DIR}/bin/python"
HTML_BUILD_DIR="${DOCS_DIR}/_build/html"

rm -rf "${DOCS_DIR}/_build"

if [[ "${BUILD_HTML}" == true ]]; then
    echo "============================================================"
    echo "Building HTML documentation"
    echo "============================================================"
    "${PYTHON_BIN}" -m sphinx -b html "${DOCS_DIR}" "${HTML_BUILD_DIR}"
fi

echo
echo "============================================================"
echo "Build completed"
echo "============================================================"
if [[ "${BUILD_HTML}" == true ]]; then
    echo "HTML:"
    echo "  ${HTML_BUILD_DIR}/index.html"
fi

if [[ "${LAUNCH_HTML}" == true ]]; then
    echo
    echo "Opening HTML documentation..."
    HTML_INDEX="${HTML_BUILD_DIR}/index.html"
    DOCS_BROWSER="${DOCS_BROWSER:-google-chrome}"
    if command -v "${DOCS_BROWSER}" >/dev/null 2>&1; then
        if "${DOCS_BROWSER}" "file://${HTML_INDEX}" >/dev/null 2>&1; then
            :
        elif "${DOCS_BROWSER}" "${HTML_INDEX}" >/dev/null 2>&1; then
            :
        elif command -v xdg-open >/dev/null 2>&1 && xdg-open "${HTML_INDEX}" >/dev/null 2>&1; then
            :
        elif command -v gio >/dev/null 2>&1 && gio open "${HTML_INDEX}" >/dev/null 2>&1; then
            :
        elif command -v python3 >/dev/null 2>&1 && python3 -m webbrowser "file://${HTML_INDEX}" >/dev/null 2>&1; then
            :
        else
            echo "Could not auto-open browser."
            echo "Open manually: ${HTML_INDEX}"
        fi
    elif command -v xdg-open >/dev/null 2>&1; then
        if xdg-open "${HTML_INDEX}" >/dev/null 2>&1; then
            :
        elif command -v gio >/dev/null 2>&1 && gio open "${HTML_INDEX}" >/dev/null 2>&1; then
            :
        elif command -v python3 >/dev/null 2>&1 && python3 -m webbrowser "file://${HTML_INDEX}" >/dev/null 2>&1; then
            :
        else
            echo "Could not auto-open browser."
            echo "Open manually: ${HTML_INDEX}"
        fi
    elif command -v gio >/dev/null 2>&1; then
        if gio open "${HTML_INDEX}" >/dev/null 2>&1; then
            :
        elif command -v python3 >/dev/null 2>&1 && python3 -m webbrowser "file://${HTML_INDEX}" >/dev/null 2>&1; then
            :
        else
            echo "Could not auto-open browser."
            echo "Open manually: ${HTML_INDEX}"
        fi
    elif command -v python3 >/dev/null 2>&1; then
        if ! python3 -m webbrowser "file://${HTML_INDEX}" >/dev/null 2>&1; then
            echo "Could not auto-open browser."
            echo "Open manually: ${HTML_INDEX}"
        fi
    else
        echo "Could not auto-open browser."
        echo "Open manually: ${HTML_INDEX}"
    fi
fi
