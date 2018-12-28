#### Installation for Ubuntu

[![Macos](https://img.shields.io/badge/MacOS-PASSED-GREEN.svg)](INSTALL_MacOS.md) [![Ubuntu](https://img.shields.io/badge/Ubuntu-PASSED-GREEN.svg)](INSTALL_Ubuntu.md) [![MinGW](https://img.shields.io/badge/MinGW-PROCESSING-LIGHTCORAL.svg)](INSTALL_MinGW.md)

##### Dependency:

+ [MinGW](http://www.mingw.org/)
    + [MinGW_build](https://sourceforge.net/projects/mingwbuilds/files/host-windows/releases/4.8.1/64-bit/threads-posix/) has posix-thread for dlib
+ [OpenCV](https://github.com/opencv/opencv/releases)
+ [dlib](https://github.com/davisking/dlib) for face detecting
+ [SigPack](http://sigpack.sourceforge.net/build.html) for C++ Signal Processing
    + [Armadillo](http://arma.sourceforge.net/) matrix math and functions likely to Matlab's
+ [Qt](https://www.qt.io/) for cross platform GUI

##### Dependency installation

+ Install [MinGW](http://www.mingw.org/) by official steps, and extract [MinGW_build](https://sourceforge.net/projects/mingwbuilds/files/host-windows/releases/4.8.1/64-bit/threads-posix/)  to your MinGW installation path 
+ OpenCV should be built by MinGW instead of prebuilt VS version
+ Install [dlib](https://github.com/davisking/dlib) with official steps
+ Install [Armadillo](http://arma.sourceforge.net/) and then you can use SigPack in `dep/sigpack`
+ Install `Qt` by official steps

##### Program build

``` shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

To get custom build settings work, change `CMakeLists.txt` with backup :smile:.