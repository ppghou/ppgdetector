#### Installation for Ubuntu

[![Macos](https://img.shields.io/badge/MacOS-PASSED-GREEN.svg)](INSTALL_MacOS.md) [![Ubuntu](https://img.shields.io/badge/Ubuntu-PASSED-GREEN.svg)](INSTALL_Ubuntu.md) [![MinGW](https://img.shields.io/badge/MinGW-PROCESSING-LIGHTCORAL.svg)](INSTALL_MinGW.md)


##### Dependency:

+ [OpenCV](https://github.com/opencv/opencv/releases)
+ [dlib](https://github.com/davisking/dlib) for face detecting
+ [SigPack](http://sigpack.sourceforge.net/build.html) for C++ Signal Processing
    + [Armadillo](http://arma.sourceforge.net/) matrix math and functions likely to Matlab's
+ [Qt](https://www.qt.io/) for cross platform GUI

##### Dependency installation

+ OpenCV by `sudo apt-get install opencv`

+ Install [dlib](https://github.com/davisking/dlib) with official steps
+ Install [Armadillo](http://arma.sourceforge.net/) and then you can use SigPack in `dep/sigpack`
+ Install `Qt` by `sudo apt-get install qt5-default qtcreator`

##### Program build

``` shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```