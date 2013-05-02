#!/bin/bash
cd riack/
mv src/CMakeLists.txt src/CMakeLists.txt.old
sed -e "s/8081/$2/g" -e "s/127.0.0.1/$1/g" ./src/CMakeLists.txt.old > ./src/CMakeLists.txt
cmake -D BUILD_SHARED_LIBS:BOOL=OFF src/
make clean all
exit $?
