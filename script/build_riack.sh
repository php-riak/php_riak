#!/bin/bash
cd riack/
if [[ -n "$1" ]]; then
  mv src/CMakeLists.txt src/CMakeLists.txt.1
  sed -e "s/127.0.0.1/$1/g" ./src/CMakeLists.txt.1 > ./src/CMakeLists.txt
  rm src/CMakeLists.txt src/CMakeLists.txt.1
  if [[ -n "$2" ]]; then
    mv src/CMakeLists.txt src/CMakeLists.txt.1
    sed -e "s/8081/$2/g" ./src/CMakeLists.txt.1 > ./src/CMakeLists.txt
    rm src/CMakeLists.txt src/CMakeLists.txt.1
  fi
fi
cmake -D BUILD_SHARED_LIBS:BOOL=OFF src/
make clean all
exit $?
