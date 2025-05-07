cmake_minimum_required(VERSION 3.25)
project(epinetcpp2)

set(CMAKE_CXX_STANDARD 17)

add_executable(epinetcpp2 main.cpp Simulation.cpp Simulation.h Common.h util.h util.cpp include/CLI11.hpp
        infection.h)
