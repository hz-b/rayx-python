cmake_minimum_required(VERSION 3.25.2)

# if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install prefix" FORCE)
# endif()

project(rayx-python)
add_subdirectory(extern)
add_subdirectory(src rayx)

# Install Python package
# install(DIRECTORY python/rayx DESTINATION .)

