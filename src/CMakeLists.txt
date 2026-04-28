project(rayxpy LANGUAGES CXX CUDA)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

if (CMAKE_VERSION VERSION_LESS 3.18)
  set(DEV_MODULE Development)
else()
  set(DEV_MODULE Development.Module)
endif()

find_package(Python 3.9 COMPONENTS Interpreter ${DEV_MODULE} REQUIRED)

# Changed from 'rayx' to 'core' to make it a private module
nanobind_add_module(core main.cpp)
nanobind_add_stub(
  core_stub
  MODULE core
  OUTPUT core.pyi
  PYTHON_PATH $<TARGET_FILE_DIR:core>
  DEPENDS core
)
target_link_libraries(core PRIVATE rayx-core)
target_include_directories(core PRIVATE 
    $<TARGET_PROPERTY:rayx-core,INTERFACE_INCLUDE_DIRECTORIES> 
    ${CUDA_TOOLKIT_INCLUDE})

add_custom_command(
  TARGET core 
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    #"${CMAKE_CURRENT_SOURCE_DIR}/extern/rayx/Data"
    "${RAYX_SOURCE_DIR}/Data"
    "${CMAKE_CURRENT_BINARY_DIR}/share/RAYX/Data"
)

file(GLOB_RECURSE PY_SRC *.py)
# file(COPY PY_SRC ".")

add_custom_command(
  TARGET core
  COMMAND ${CMAKE_COMMAND} -E copy
    ${PY_SRC}
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Install to rayx/ subdirectory
install(TARGETS core LIBRARY DESTINATION rayx)
