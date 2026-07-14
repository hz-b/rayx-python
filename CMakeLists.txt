# 3.26 is required for FindPython's Development.SABIModule component, which the
# STABLE_ABI extension build depends on (see nanobind_add_module below).
cmake_minimum_required(VERSION 3.26)
project(rayx-python LANGUAGES CXX)

# rayx-core + nanobind. Provides the `rayx-core` target and the nanobind_add_*
# helpers (both stay available at this scope after the subdirectory is processed).
add_subdirectory(extern EXCLUDE_FROM_ALL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(alpaka_ACC_GPU_CUDA_ENABLE)
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 20)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
endif()
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

# Development.SABIModule provides the Python::SABIModule target that nanobind's
# STABLE_ABI build requires; without it nanobind silently falls back to a
# version-specific extension.
find_package(Python 3.12 COMPONENTS Interpreter Development.Module Development.SABIModule REQUIRED)

# The compiled extension is implementation-private (`_core`); the Python package
# `src/rayx` owns the public API. Following nanobind's recommended layout, the
# binding source sits next to the package as `src/rayx_ext.cpp`.
#
# STABLE_ABI builds against the CPython 3.12+ limited API (abi3) so a single wheel
# covers every supported interpreter (3.12, 3.13, 3.14, ...) instead of one per
# minor version. Paired with `wheel.py-api = "cp312"` in pyproject.toml for tagging.
nanobind_add_module(_core STABLE_ABI src/rayx_ext.cpp)
nanobind_add_stub(
  _core_stub
  MODULE _core
  OUTPUT _core.pyi
  PYTHON_PATH $<TARGET_FILE_DIR:_core>
  DEPENDS _core
)
target_link_libraries(_core PRIVATE rayx-core)
target_include_directories(_core PRIVATE
    $<TARGET_PROPERTY:rayx-core,INTERFACE_INCLUDE_DIRECTORIES>)

# Bundle rayx-core's runtime Data into the wheel. At import time the module
# registers its own package directory as the resource lookup path.
add_custom_command(
  TARGET _core POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${RAYX_SOURCE_DIR}/Data"
    "${CMAKE_CURRENT_BINARY_DIR}/share/RAYX/Data"
)

# Install native artifacts into the Python package. scikit-build-core installs
# the Python sources from src/rayx independently (see pyproject wheel.packages).
install(TARGETS _core DESTINATION rayx)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/_core.pyi" DESTINATION rayx)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/share/RAYX/Data" DESTINATION rayx/share/RAYX)
