#! /bin/bash

# Get the number of cores
numCores=`grep -c ^processor /proc/cpuinfo`

cd ../data
time ../src/rt -c $numCores ${1}.lua 
xdg-open ${1}.png
cd ../src
