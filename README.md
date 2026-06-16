Reimplementation of the engine of アストラエアの白き永遠 for Linux systems.

## Installation
#### Dependencies (Ubuntu):
The following dependencies need to be installed before configuring with cmake:
```
cmake
libogg
```

As part of the configuration step, cmake fetches the following dependencies
```
SDL3
zstr
googletest
libvorbis
```

In addition to these libraries, it is also a requirement that the game files are present when running the program. How you get this are up to you - Support the studio :)
#### Configuring & Compiling
Run:
```
`cmake -S . -B <desired-output-folder>`
cmake --build <desired-output-folder>
```
After compilation, simply run
```
./<desired-output-folder>/AstralAir
```

