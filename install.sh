#!/bin/bash

sudo aptitude update
sudo aptitude -y install cmake build-essential libjsoncpp-dev git
git clone "https://github.com/Aerial94/TIN" dns-checker
cd dns-checker
echo -e "\e[36mCreate build dir\e[0m"
mkdir build
cd build
echo -e "\e[36mInvoking cmake\e[0m"
cmake ..
echo -e "\e[36mInvoking make\e[0m"
make dns-checker
cd ..
echo -e "\e[36mCreate bin dir\e[0m"
mkdir bin
echo -e "\e[36mCoping dns-checker binary to bin dir\e[0m"
cp build/dns-checker bin
echo -e "\e[36mCoping configuration to bin dir\e[0m"
cp conf/configuration.json bin
echo -e "\e[36mEntering bin dir\e[0m"
cd bin
