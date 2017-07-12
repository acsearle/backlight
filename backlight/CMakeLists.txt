cmake_minimum_required(VERSION 3.2.2)

project(Backlight)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 11)
include_directories("${CMAKE_SOURCE_DIR}/../")
find_package(Boost REQUIRED)
include_directories("${Boost_INCLUDE_DIRS}")

set(SOURCES
  src/Bitmap.cpp
  src/Bounding_Sphere.cpp
  src/Box.cpp
  src/CSG.cpp
  src/Camera.cpp
  src/Colour.cpp
  src/Cone.cpp
  src/Cylinder.cpp
  src/Display.cpp
  src/Event.cpp
  src/Frame.cpp
  src/Global.cpp
  src/Keywords.cpp
  src/Light_Source.cpp
  src/Main.cpp
  src/Map.cpp
  src/Matrix.cpp
  src/Object.cpp
  src/Plane.cpp
  src/Quadric.cpp
  src/Ray.cpp
  src/Scene.cpp
  src/Smooth_Triangle.cpp
  src/Sphere.cpp
  src/Texture.cpp
  src/Tokenizer.cpp
  src/Transform.cpp
  src/Triangle.cpp
  src/Vector.cpp
  src/Warps.cpp)

add_executable(backlight ${SOURCES})
