#!/bin/sh
# This script builds llmozlib itself.  This should happen after
# mozilla has been built and copy_products_linux.sh has been run.

CXX='g++-3.4'
MOZARCH='i686-linux'
SRCS='llembeddedbrowser.cpp llembeddedbrowserwindow.cpp llmozlib.cpp'

#------------------------

LIBNAME=libllmozlib
OBJS=`echo ${SRCS} | sed s/\\.cpp/.o/g`
INCS_LINE="`pkg-config gtk+-2.0 --cflags` `find libraries/${MOZARCH}/include -type d | sed s/^/-I/`"
DEFS='-DMOZILLA_INTERNAL_API -DLL_LINUX=1'
OPTS='-ggdb'

rm -f ${LIBNAME}.a ${OBJS}

for source in ${SRCS} ; do
    ${CXX} ${OPTS} ${DEFS} ${INCS_LINE} -c ${source}
done

ar rcs ${LIBNAME}.a ${OBJS}

ls -la ${LIBNAME}.a


