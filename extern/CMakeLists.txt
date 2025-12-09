set(RAYX_STATIC_LIB ON)
set(RAYX_BUILD_RAYX_CLI NO)
set(RAYX_BUILD_RAYX_UI NO)
set(RAYX_CUSTOM_DATA_DIR "rayxdata/share" CACHE STRING "Set this val")

add_subdirectory(rayx)
add_subdirectory(pybind11)