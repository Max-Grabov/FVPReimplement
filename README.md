Reimplementation of the FVP engine from scratch for Linux and other SDL3 supported systems.

## Installation
#### Dependencies (Ubuntu):
A prerequisite is that you'll need a C++20 compiler and CMake ≥ 3.20
The following dependencies need to be installed before configuring with cmake:
```
libogg
```

As part of the configuration step, cmake fetches the following dependencies
- [SDL3](https://github.com/libsdl-org/SDL) (`release-3.4.8`)
- [libvorbis](https://github.com/xiph/vorbis) (`v1.3.7`)
- [zstr](https://github.com/mateidavid/zstr)

In addition to these libraries, it is also a requirement that the game files are present when running the program. How you get this are up to you - Support the studio :)
#### Configuring & Compiling
Run:

`cmake -S . -B <desired-output-folder>`

For test configuration:

`cmake -S . -B <desired-output-folder> -DBUILD_TESTS=ON`

Then:

`cmake --build <desired-output-folder>`

After compilation, simply run

`./<desired-output-folder>/AstralAir`

