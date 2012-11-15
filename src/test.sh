#! /bin/bash

cd ../data
../src/rt ${1}.lua 
display ${1}.png
cd ../src
