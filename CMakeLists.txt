cmake_minimum_required(VERSION 2.8.12)

find_package(opencv REQUIRED)
add_subdirectory(dep/dlib dlib_build)

macro(add_example name)
   add_executable(${name} src/${name}.cpp)
   target_link_libraries(${name} dlib::dlib ${OpenCV_LIBS})
endmacro()

macro(add_gui_example name)
   if (DLIB_NO_GUI_SUPPORT)
      message("No GUI support, so we won't build the ${name} example.")
   else()
      add_example(${name})
   endif()
endmacro()

add_example(ppgDetector)