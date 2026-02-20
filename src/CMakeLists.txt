project(rayxpy LANGUAGES CXX CUDA)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(PYBIND11_FINDPYTHON ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

# Changed from 'rayx' to '_core' to make it a private module
pybind11_add_module(_core main.cpp)
target_link_libraries(_core PRIVATE rayx-core)
target_include_directories(_core PRIVATE 
    $<TARGET_PROPERTY:rayx-core,INTERFACE_INCLUDE_DIRECTORIES> 
    ${CUDA_TOOLKIT_INCLUDE}/cccl)

# Install to rayx/ subdirectory
install(TARGETS _core LIBRARY DESTINATION rayx)
