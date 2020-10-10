cmake_minimum_required(VERSION 3.17)
project(drawing-fractal)

set(CMAKE_CXX_STANDARD 17)

set(LINKER_FLAGS "-static-libgcc -static-libstdc++ -Wl,-Bstatic
    -lstdc++ -lpthread -Wl,-Bdynamic -lshlwapi
    -lws2_32 -Wint-to-pointer-cast -Wcpp -lws2_32 -lIphlpapi -lpsapi")

add_executable(drawing-fractal main.cpp BitMap.h BitMap.cpp BitMapFile.h BitMapInfo.h Colouring.hpp Colouring.cpp FractalCreator.cpp FractalCreator.hpp Mandelbrot.cpp Mandelbrot.hpp
        Zoom.h ZoomList.hpp ZoomList.cpp)

target_link_libraries(drawing-fractal ${LINKER_FLAGS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
