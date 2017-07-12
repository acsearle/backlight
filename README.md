# Backlight
A raytracer for demonstrating special-relativistic effects using a Constructive Solid Geometry scene description language.

## Building on Windows
To build on Windows, you can use either the Visual Studio solution files in the `backlight/` folder
or generate new solution files using the CMake GUI.

## Building on Linux
To build on Linux, use the CMakeLists in the `backlight/` folder:
```shell
cd backlight
mkdir build
cmake ..
make
```

## Running
To run, provide a scene description as the first argument on the command line. The `backlight/srs/` folder contains
a few descriptions.
