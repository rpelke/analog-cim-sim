#!/bin/bash
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do 
    DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

PROJ_PATH=$DIR/..
BUILD_PATH=$PROJ_PATH/build/debug/build
REL_PATH=$PROJ_PATH/build/release/build

mkdir -p $BUILD_PATH && cd $BUILD_PATH
cmake -DCMAKE_INSTALL_PREFIX=$BUILD_PATH/.. -DCMAKE_BUILD_TYPE=Debug $PROJ_PATH/cpp
make && make install
cd $DIR

mkdir -p $REL_PATH && cd $REL_PATH
cmake -DCMAKE_INSTALL_PREFIX=$REL_PATH/.. -DCMAKE_BUILD_TYPE=Release $PROJ_PATH/cpp
make && make install
cd $DIR
