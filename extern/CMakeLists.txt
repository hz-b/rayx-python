set(RAYX_STATIC_LIB ON)
set(VULKAN NO)
set(RAYX_CMD NO)
set(RAYX_CUSTOM_DATA_DIR "rayxdata/share" CACHE STRING "Set this val")

add_subdirectory(rayx)
add_subdirectory(pybind11)