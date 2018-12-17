# PPG Viewer CMakeLists
# Copyright (C) 2018 PPG Hou <https://github.com/ppghou>

cmake_minimum_required(VERSION 3.0.2)
cmake_policy(VERSION 3.0.2)
project(ppgViewer VERSION 1.0)

# = = = = = = = = = = = = = = = = = = = = = = = = 
# Setting
# = = = = = = = = = = = = = = = = = = = = = = = = 
add_definitions(-Dunix -DHAVE_FFTW)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_SRC_PATH "${CMAKE_SOURCE_DIR}/src")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# UNINSTALL setting
configure_file(
    "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")

# *UNIX setting
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra")
else()
    set(WINDOWS_AUTHOR "PPG Hou")
    set(WINDOWS_DESCRIPTION "${PROJECT_NAME_LOWER}")
    set(WINDOWS_COPYRIGHT "Copyright (C) 2018 PPG Hou")
endif()

# Statically link with libgcc
if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nologo -Zc:wchar_t")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nologo -Zc:wchar_t -w34100 -w34189")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NOLOGO /DYNAMICBASE /NXCOMPAT")
endif()

# Make final release build smaller
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s")
endif()

# = = = = = = = = = = = = = = = = = = = = = = = = 
# DEPENDENCY
# = = = = = = = = = = = = = = = = = = = = = = = = 
# DEPENDENCY: opencv
find_package(opencv REQUIRED)

# DEPENDENCY: dlib
add_subdirectory(dep/dlib dlib_build)

# DEPENDENCY: qt
find_package(Qt5 5.2.0 REQUIRED COMPONENTS Quick Core Charts Widgets)
find_package(VLCQt REQUIRED COMPONENTS Core Widgets)
# Set Qt bin dir to find QtCore*.dll and other libs to install
string(REGEX REPLACE "([^ ]+)[/\\].*" "\\1" QT_BIN_DIR_TMP "${QT_MOC_EXECUTABLE}")
string(REGEX REPLACE "\\\\" "/" QT_BIN_DIR "${QT_BIN_DIR_TMP}")
string(REGEX REPLACE "bin" "lib" QT_LIB_DIR "${QT_BIN_DIR}")

# DEPENDENCY: sigpack
include_directories(
    ${CMAKE_SOURCE_DIR}/dep/sigpack)

# = = = = = = = = = = = = = = = = = = = = = = = = 
# INSTALL PREPARATION
# = = = = = = = = = = = = = = = = = = = = = = = = 

# INSTALL setting
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
set(PROJECT_INSTALL_BIN_DIR "bin")
set(PROJECT_INSTALL_LIB_DIR "lib")

# MacOS setting
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(PROJECT_INSTALL_BIN_DIR "${PROJECT_NAME}.app/Contents/MacOS")
    set(PROJECT_INSTALL_LIB_DIR "${PROJECT_NAME}.app/Contents/MacOS/lib")
    set(CMAKE_INSTALL_RPATH "@executable_path/lib")
    set(MACOSX_BUNDLE_COPYRIGHT "2018 PPG Hou")
    set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_NAME}.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME_LOWER}")
endif()

# Include source
if(MINGW OR MSVC)
    add_subdirectory(windows)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_subdirectory(osx)
endif()

# = = = = = = = = = = = = = = = = = = = = = = = = 
# BUILD
# = = = = = = = = = = = = = = = = = = = = = = = = 
set(UI
    ${CMAKE_SRC_PATH}/ppgViewer.ui)
set(SRC
    ${CMAKE_SRC_PATH}/main.cpp
    ${CMAKE_SRC_PATH}/ppgViewer.cpp)

# BUILD: for dlib dependent source file
macro(add_dlib name)
    add_executable(${name} ${CMAKE_SRC_PATH}/${name}.cpp)
    target_link_libraries(${name} dlib::dlib ${OpenCV_LIBS} -lgfortran)
endmacro()

# BUILD: for sigpack dependent source file
# g++ -Wall -std=c++11 -Dunix -DHAVE_FFTW -I../sigpack -larmadillo -lfftw3
macro(add_sigpack name)
   add_executable(${name} ${CMAKE_SRC_PATH}/${name}.cpp)
endmacro()

# BUILD: main interface and all dependent files
macro(add_main name)
    add_executable(${name}
        ${UI}
        ${SRC}
    )
    qt5_use_modules(${name}
        Quick Core Charts Widgets
    )
    target_link_libraries(${name}
        -lgfortran
        dlib::dlib
        ${OpenCV_LIBS}
        Qt5::Core Qt5::Charts Qt5::Widgets
        VLCQt::Core VLCQt::Widgets
    )
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        add_custom_command(TARGET ${name} 
            POST_BUILD COMMAND
            ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "/usr/local/opt/qt5/lib" ${name})
    endif()
endmacro()

# BUILD: main interface and all dependent files
macro(add_app name)
    # Compile resources with windres
    if(MINGW)
        add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/windows/Project.obj
            COMMAND ${CMAKE_RC_COMPILER}
                -i ${CMAKE_BINARY_DIR}/windows/Project.rc
                -o ${CMAKE_BINARY_DIR}/windows/Project.obj
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/windows")
        set(SRC ${SRC} ${CMAKE_BINARY_DIR}/windows/Project.obj)
    endif()
    if(MSVC)
        set(SRC ${SRC} ${CMAKE_BINARY_DIR}/windows/Project.rc)
    endif()
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(ICONS ${CMAKE_SOURCE_DIR}/osx/Project.iconset)
        file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Resources)
        find_program(ICONUTIL_EXECUTABLE NAMES iconutil)
        add_custom_command(
            OUTPUT ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Resources/${PROJECT_NAME}.icns
            COMMAND ${ICONUTIL_EXECUTABLE} --convert icns --output ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Resources/${PROJECT_NAME}.icns
            ${ICONS}
        )
        set(SRC ${SRC} ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Resources/${PROJECT_NAME}.icns)
    endif()

    add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32 ${UI} ${SRC})
    
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "${QT_LIB_DIR};@executable_path/../Frameworks")
        set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_BINARY_DIR}/osx/Info.plist")
    endif()

    qt5_use_modules(${name}
        Quick Core Charts Widgets
    )
    target_link_libraries(${PROJECT_NAME}
        -lgfortran
        dlib::dlib
        ${OpenCV_LIBS}
        VLCQt::Core VLCQt::Widgets)
    install(
        TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${PROJECT_INSTALL_BIN_DIR}
        BUNDLE DESTINATION .
    )
endmacro()

# = = = = = = = = = = = = = = = = = = = = = = = = 
# EXEC
# = = = = = = = = = = = = = = = = = = = = = = = = 

# EXEC: test
# add_dlib(ppgDetector)
# add_sigpack(ppgFilter)

# EXEC: main
add_main(ppgViewer)

# EXEC: app
# add_app(ppgViewer)
