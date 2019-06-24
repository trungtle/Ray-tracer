#!/bin/sh

# compile Raytracer
mkdir build
cd build
cmake -G "Unix Makefiles" ../
make

echo "Build success. Run ray tracer..."

#launch
./Raytracer