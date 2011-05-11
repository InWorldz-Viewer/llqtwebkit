#!/bin/bash


QT_KOKUA_DIR="/src/qt-kokua"
QT_SOURCE_DIR="$QT_KOKUA_DIR/qt"
#QT_SOURCE_DIR="/src/libs-kokua/3p-qt/qt-everywhere-opensource-src-4.7.1"

stage="$(pwd)/stage"
packages="$stage/packages"
install="$stage"

cd "$(dirname "$0")"



if ( [ -f Makefile ] && [ $(stat -c%s "Makefile") != 0 ] ); then
    make clean
fi

export QTDIR="$QT_KOKUA_DIR/stage"
export PATH="$QTDIR/bin:$PATH"

qmake  CONFIG-=debug
perl -pi -e 's/-O2 -fPIC/-O2 -fno-stack-protector -fPIC/' Makefile
make -j4


mv "$install/lib" "$install/release"
mkdir -p "$install/lib"
mv "$install/release" "$install/lib"

LIB_DIR="$install/lib/release"
cp "libllqtwebkit.a" "$LIB_DIR"

mkdir -p "$install/include"
cp "llqtwebkit.h" "$install/include"

mv "$stage/plugins/imageformats"/libq*.a "$LIB_DIR"

mkdir -p "$install/LICENSES"
cp "LLQTWEBKIT_LICENSE.txt" "$install/LICENSES/llqtwebkit.txt"
