# PPG Viewer CMakeLists
# Copyright (C) 2018 PPG Hou <https://github.com/ppghou>

# Windows Extra

if(MINGW OR MSVC)
    configure_file(
        ${CMAKE_SOURCE_DIR}/windows/Project.rc.in
        ${CMAKE_BINARY_DIR}/windows/Project.rc
    )
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(VLCQtPostfix "d")
endif()
if(MINGW)
    set(VLCQtPrefix "lib")
endif()

set(VLCQtLibs
    ${VLCQtCoreLocation}/bin/${VLCQtPrefix}VLCQtCore${VLCQtPostfix}.dll
    ${VLCQtWidgetsLocation}/bin/${VLCQtPrefix}VLCQtWidgets${VLCQtPostfix}.dll
    ${VLCQtCoreLocation}/bin/libvlc.dll
    ${VLCQtCoreLocation}/bin/libvlccore.dll
)

set(VLCQtPlugins ${VLCQtCoreLocation}/bin/plugins)

file(GLOB VLCGcc "${VLCQtCoreLocation}/bin/libgcc_s_*-1.dll")
if(VLCGcc)
    set(ExtraLibs ${VLCGcc})
endif()

file(GLOB VLCStd "${VLCQtCoreLocation}/bin/libstdc++-6.dll")
if(VLCStd)
    set(ExtraLibs
        ${ExtraLibs}
        ${VLCQtCoreLocation}/bin/libstdc++-6.dll
        ${VLCQtCoreLocation}/bin/libwinpthread-1.dll
    )
endif()

add_custom_target(windows
    "${QT_BIN_DIR}/windeployqt.exe" "${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_BIN_DIR}/${PROJECT_NAME}.exe")


################
# Installation #
################
install(FILES ${VLCQtLibs} DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_BIN_DIR}")
install(DIRECTORY ${VLCQtPlugins} DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_BIN_DIR}")

if(VLCGcc OR VLCStd)
    install(FILES ${ExtraLibs} DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_BIN_DIR}")
endif()
