# Mobile/PC Markerless AR tracker app

C++/CMake core project for **Markerless AR** detector&tracker app powered by OpenCV.

Currently have PC and Android version (JNI).

### Based on [this project](https://github.com/takmin/OpenCV-Marker-less-AR)

## Built With

* CMake 3.6 / C++17
* OpenCV 4.2.0 (PC and Android)
* Android NDK 20

## Tests

### Markers
<p align="center">
<img src="https://raw.githubusercontent.com/khoben/ar.core/master/README.md-images/czech.jpg" width="300" height="300">
<img src="https://raw.githubusercontent.com/khoben/ar.core/master/README.md-images/miku.jpg" width="300" height="300">
</p>

### Recognition
#### Single

<p align="center">
<img src="https://raw.githubusercontent.com/khoben/ar.core/master/README.md-images/2.png" width="300">
<img src="https://raw.githubusercontent.com/khoben/ar.core/master/README.md-images/1.png" width="300">
</p>

#### Multi

<p align="center">
<img src="https://raw.githubusercontent.com/khoben/ar.core/master/README.md-images/multi-multi.png" width="600">
</p>

## Known issues
* Sometimes it calculates incorrect object`s coordinates on tracking phase

