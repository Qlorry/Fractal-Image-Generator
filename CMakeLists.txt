cmake_minimum_required(VERSION 3.17)
project(drawing-fractal)

set(CMAKE_CXX_STANDARD 17)

add_executable(drawing-fractal main.cpp BitMap.h BitMap.cpp BitMapFile.h BitMapInfo.h Colouring.hpp Colouring.cpp FractalCreator.cpp FractalCreator.hpp Mandelbrot.cpp Mandelbrot.hpp
        Zoom.h ZoomList.hpp ZoomList.cpp)


#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
