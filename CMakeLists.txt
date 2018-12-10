cmake_minimum_required(VERSION 2.8.12)

find_package(opencv REQUIRED)
add_subdirectory(dep/dlib dlib_build)
add_definitions(-Dunix -DHAVE_FFTW)

include_directories(dep/sigpack)
link_directories()

# for dlib dependent source file
macro(add_dlib name)
   add_executable(${name} src/${name}.cpp)
   target_link_libraries(${name} dlib::dlib ${OpenCV_LIBS})
endmacro()

# for sigpack dependent source file
# g++ -Wall -std=c++11 -Dunix -DHAVE_FFTW -I../sigpack -larmadillo -lfftw3
macro(add_sigpack name)
   add_executable(${name} src/${name}.cpp)
   target_link_libraries(${name} )
endmacro()

add_dlib(ppgDetector)
add_sigpack(signalTester)