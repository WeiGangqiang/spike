#!/bin/bash

target="build"

if [ ! -d $target ]; then
    mkdir -p $target
fi

cd $target

echo "*******************************************************************************"
echo "start to build demo ..."


cmake ..
cmake --build .

if [ $? -ne 0 ]; then
    echo "FAILED!"
    cd ..
    exit 1
fi

echo "*******************************************************************************"
echo "begin feature test ..."
echo "*******************************************************************************"

#./test/ut/MemBuf_test
./test/b