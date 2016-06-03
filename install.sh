#!/bin/bash

sudo aptitude update
sudo aptitude -y install cmake build-essential libjsoncpp-dev git
git clone "https://github.com/Aerial94/TIN" dns-checker
cd dns-checker
mkdir build
cd build
cmake ..
make dns-checker
