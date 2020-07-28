#!/bin/bash

if icpc --version; then
    compilers=(g++ clang++ icpc)
else
    compilers=(g++ clang++)
fi

for cxx in $compilers
do
    for std in 14 17
    do
        echo "Testing $cxx c++$std..."
        make CXX="$cxx" STD="$std"
        [[ $? != 0 ]] && echo "Testing $cxx c++$std failed. Exiting..." && exit 1
    done
done

echo "All tests passed."


