cmake_minimum_required(VERSION 3.25.2)

project(rayx-python)

add_subdirectory(extern)
add_subdirectory(src)

# create __init__.py and install it in INSTALL_DIR/rayxdata
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/__init__.py" "")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/__init__.py" DESTINATION "rayxdata")
