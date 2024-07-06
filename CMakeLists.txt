cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0074 NEW)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

project ("FunctionSubstitutionCore")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Python REQUIRED Development Interpreter)
if(${Python_FOUND})
    message("Found Python ${Python_VERSION}")
    get_filename_component(Python_ROOT_DIR ${Python_ROOT_DIR} ABSOLUTE)
    message("Python ROOT ${Python_ROOT_DIR}")
    message("Python include ${Python_INCLUDE_DIRS}")
    message("Python lib ${Python_LIBRARIES}")
    message("Python Configuration Done\n")
endif()

set(Boost_USE_STATIC_LIBS ON)
add_compile_definitions(BOOST_PYTHON_STATIC_LIB)
find_package(Boost REQUIRED "python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")
if(${Boost_FOUND})
    message("Found Boost ${Boost_VERSION}")
    message("Boost include: ${Boost_INCLUDE_DIRS}")
    message("Boost lib dir: ${Boost_LIBRARY_DIRS}")
    message("Boost lib: ${Boost_LIBRARIES}")
    message("Boost Configuration Done\n")
endif()

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

add_subdirectory(src)

#Copying Python dll
add_custom_command(
    OUTPUT "${CMAKE_PROJECT_NAME}/python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.dll"
    COMMENT "Copying python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${Python_ROOT_DIR}/python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.dll"
    ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}
    DEPENDS "${Python_ROOT_DIR}/python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.dll"
    VERBATIM
)
add_custom_target(copy_python_dll DEPENDS "${CMAKE_PROJECT_NAME}/python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.dll")
add_dependencies(${CMAKE_PROJECT_NAME} copy_python_dll)

#Copying Python STDLIBS
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/python/DLLs)
file(GLOB Python_STDLIBS ${Python_ROOT_DIR}/DLLs/*.*)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/python/DLLs/_ctypes.pyd
    COMMENT "Copying Python DLLs"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${Python_STDLIBS}
    ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/python/DLLs
    DEPENDS ${Python_STDLIBS}
    VERBATIM
)
add_custom_target(copy_python_stdlibs DEPENDS ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/python/DLLs/_ctypes.pyd)
add_dependencies(${CMAKE_PROJECT_NAME} copy_python_stdlibs)

#Copying Python Lib
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/python/Lib)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/python/Lib/os.py
    COMMENT "Copying Python Lib in ${Python_ROOT_DIR}/Lib"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${Python_ROOT_DIR}/Lib ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/python/Lib
    DEPENDS ${Python_ROOT_DIR}/Lib/os.py
    VERBATIM
)
add_custom_target(copy_python_lib DEPENDS ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/python/Lib/os.py)
add_dependencies(${CMAKE_PROJECT_NAME} copy_python_lib)

#Install site-package
add_custom_command(
    OUTPUT pip-site-packages.txt
    COMMENT "Install python site-packages"
    DEPENDS ${CMAKE_SOURCE_DIR}/pip-site-packages.txt
    COMMAND ${Python_EXECUTABLE} -m pip install -r ${CMAKE_SOURCE_DIR}/pip-site-packages.txt --target ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/python/Lib/site-packages --no-warn-script-location
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/pip-site-packages.txt
    ${CMAKE_BINARY_DIR}/pip-site-packages.txt
    VERBATIM
)
add_custom_target(install_pip DEPENDS pip-site-packages.txt)
add_dependencies(${CMAKE_PROJECT_NAME} install_pip)