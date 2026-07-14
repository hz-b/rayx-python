set(RAYX_STATIC_LIB ON)
set(RAYX_BUILD_RAYX_CLI NO)
set(RAYX_BUILD_RAYX_UI NO)
set(RAYX_BUILD_RAYX_TESTS NO)
# set(RAYX_CUSTOM_DATA_DIR "rayxdata/share" CACHE STRING "Set this val")

# rayx-core embeds CMAKE_INSTALL_PREFIX in every compilation unit. Packaging
# frontends use a new temporary wheel staging prefix for each invocation, which
# would otherwise invalidate the complete native build on every editable
# reinstall. The Python binding registers its module directory as the runtime
# resource lookup path, so rayx-core can use a stable build-local prefix while
# scikit-build-core still overrides the actual destination during installation.
set(RAYX_PYTHON_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/rayx-install")
add_subdirectory(rayx)
set(CMAKE_INSTALL_PREFIX "${RAYX_PYTHON_INSTALL_PREFIX}")
unset(RAYX_PYTHON_INSTALL_PREFIX)

add_subdirectory(nanobind)
