#! /bin/bash

cd ../data
../src/rt -s ${1}.lua 
#display ${1}.png
xdg-open ${1}.png
cd ../src
