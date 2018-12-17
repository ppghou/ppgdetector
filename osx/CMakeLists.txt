# PPG Viewer
# Copyright (C) 2018 PPG Hou <https://github.com/ppghou>

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_OSX_ARCHITECTURES x86_64)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    ADD_CUSTOM_TARGET(dmg
        "${QT_BIN_DIR}/macdeployqt"
        "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app" -dmg)
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/osx/Info.plist.in
    ${CMAKE_BINARY_DIR}/osx/Info.plist
)

# Installation
install(DIRECTORY
    ${VLCQtCoreFramework} DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks")

install(DIRECTORY
    ${VLCQtWidgetsFramework} DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks")
