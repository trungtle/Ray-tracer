#!/bin/sh

# compile Raytracer
mkdir build
cd build
cmake -G "Unix Makefiles" ../
make

#launch
./Raytracer