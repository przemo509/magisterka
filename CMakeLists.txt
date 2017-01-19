cmake_minimum_required(VERSION 3.6)
project(magisterka)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp src/utils/Config.cpp src/utils/Config.h src/utils/Logger.cpp src/utils/Logger.h src/utils/Timer.cpp src/utils/Timer.h src/simulation/ExplosionSimulation.cpp src/simulation/ExplosionSimulation.h src/simulation/Vortex.cpp src/simulation/Vortex.h src/utils/MathUtils.h src/utils/GeometryUtils.h src/utils/StringUtils.h src/utils/lib/snprintf/snprintf.c src/utils/lib/snprintf/snprintf.h src/utils/BlenderExport.h)
add_executable(magisterka ${SOURCE_FILES})