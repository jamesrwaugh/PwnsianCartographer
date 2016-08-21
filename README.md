# Pwnsian Cartographer
A simple top-down Minecraft world renderer (Anvil format)

This is a work in progress.

![World Render](NewerWorldRender.png)

##Features
- Multi-core rendering 
- Output imaging scaling (1x, 2x, ...)
- Dynamic block color generation
- Extendable and custom block support

##Building and Running
Building requires GCC 4.9 or later or any compiler with C++14 support. This project uses CMake. Additionally, a  A QtCreater project file is provided in `src`.

Go into the PwnsianCartographer folder:
```
git submodule init
git submodule update
mkdir build && cd build
cmake ..
make
```
This will produce the executable in `build/bin`

###Library Requirements
All used libraries are included as submodules, except for:
- SDL2 (http://libsdl.org/)
- libpng on *nix systems (Should probably be included with your OS)
