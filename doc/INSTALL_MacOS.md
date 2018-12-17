#### Installation for MacOS

[![Macos](https://img.shields.io/badge/MacOS-PASSED-GREEN.svg)](./doc/INSTALL_MacOS.md)  [![MinGW](https://img.shields.io/badge/MinGW-PROCESSING-LIGHTCORAL.svg)](./doc/INSTALL_MinGW.md)

##### Dependency:

+ [OpenCV](https://github.com/opencv/opencv/releases)
+ [dlib](https://github.com/davisking/dlib) for face detecting
+ [SigPack](http://sigpack.sourceforge.net/build.html) for C++ Signal Processing
    + [Armadillo](http://arma.sourceforge.net/) matrix math and functions likely to Matlab's
+ [Qt](https://www.qt.io/) for cross platform GUI
+ [vlc-qt](https://github.com/vlc-qt/vlc-qt) video player API for Qt
    + [VLC](https://www.videolan.org/vlc/) popular video player

##### Dependency installation

+ OpenCV by `brew install opencv`

+ Install [dlib](https://github.com/davisking/dlib) with official steps
+ Install [Armadillo](http://arma.sourceforge.net/) and then you can use SigPack in `dep/sigpack`
+ Install `Qt` by `brew install qt5`

+ Install VLC.app from official site
+ Install [vlc-qt](https://github.com/vlc-qt/vlc-qt) with official steps

##### Program build

``` shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

If you want to test with only executable file instead of whole `.app`, you can change the line at end of `CMakeLists.txt`.