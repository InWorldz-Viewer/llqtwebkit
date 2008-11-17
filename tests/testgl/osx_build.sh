#!/bin/bash -x

CXX=g++
INCFLAGS=-I../..

CXXOPT="-ggdb -DLL_NEWER_GLUI -DLL_OSX -L$QTDIR/lib"
LIBFLAGS="$PWD/../../libllmozlib2.a -lQtWebKit -lQtOpenGL -lQtNetwork -lQtGui -lQtCore -framework GLUT -framework OpenGL"

rm -f testgl

${CXX} ${CXXOPT} ${INCFLAGS} testgl.cpp ${LIBFLAGS} -o testgl

