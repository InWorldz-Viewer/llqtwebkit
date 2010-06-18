#!/bin/bash

# turn on verbose debugging output for parabuild logs.
set -x
# make errors fatal
set -e

LLQTWEBKIT_VERSION="4.6-cookies"
QT_REPOS="http://qt.gitorious.org/qt/lindenqt/archive-tarball/lindenqt"
QT_ARCHIVE="qt-lindenqt.tar.gz"
QT_SOURCE_DIR="qt-lindenqt"

if [ -z "$AUTOBUILD" ] ; then 
    fail
fi

if [ "$OSTYPE" = "cygwin" ] ; then
    export AUTOBUILD="$(cygpath -u $AUTOBUILD)"
fi

# load autbuild provided shell functions and variables
eval "$("$AUTOBUILD" source_environment)"

fetch_git_as_tarball()
{
  local archive=$1
  local url=$2

  # *HACK work around the fact that gitorious.org screws us by being
  # non-blocking while it's building the tarball
  # *NOTE - keep trying until it actually hands us back a tarball or 15 minutes total
  local retry_count=30
  while (($retry_count)) && ! curl -q -I "$url" 2>/dev/null | grep -E "^Content-Type:.*application\/x-gzip" ; do
    sleep 30
    let retry_count--
  done

  # by now hopefully it's actually got a tarball ready for us
  curl -q -o "$archive" "$url"
}

fetch_git_as_tarball "$QT_ARCHIVE" "$QT_REPOS"
extract "$QT_ARCHIVE"

top="$(pwd)"
case "$AUTOBUILD_PLATFORM" in
    "windows")
        build="$(pwd)/build-vc80"
        packages="$build/packages"
        install="$build/install"
        QTDIR=$(cygpath -m "$(pwd)/$QT_SOURCE_DIR")
        pushd "$QT_SOURCE_DIR"
            chmod +x "./configure.exe"
            yes | head -n 1 | \
                ./configure.exe -opensource -platform win32-msvc2008 -debug-and-release -no-qt3support -prefix "$QTDIR" -qt-libjpeg -qt-libpng -openssl-linked -no-plugin-manifests -I "$packages/include" -L "$packages/lib/release"
            export PATH="$(cygpath -u "$QTDIR")/bin:$PATH"
            export QMAKESPEC="win32-msvc2008"

            nmake sub-src
        popd

        qmake "CONFIG-=debug" && nmake
        qmake "CONFIG+=debug" && nmake

        mkdir -p install/lib/debug
        mkdir -p install/lib/release
        cp "contrib/vstudio/vc8/x86/ZlibStatDebug/zlibstat.lib" \
            "install/lib/debug/zlibd.lib"
        cp "contrib/vstudio/vc8/x86/ZlibStatRelease/zlibstat.lib" \
            "install/lib/release/zlib.lib"
        mkdir -p "install/include/zlib"
        cp {zlib.h,zconf.h} "install/include/zlib"

        local qtwebkit_libs_debug="QtCored4.dll QtCored4.lib QtGuid4.dll QtGuid4.lib \
            qtmain.lib QtNetworkd4.dll QtNetworkd4.lib QtOpenGLd4.dll QtOpenGLd4.lib \
            QtWebKitd4.dll QtWebKitd4.lib"

        for lib in $qtwebkit_libs_debug ; do
            cp "$QTWEBKIT_DIR/lib/$lib" "$DEBUG_OUT_DIR"
        done

        local qtwebkit_libs_release="QtCore4.dll QtCore4.lib QtGui4.dll QtGui4.lib \
            qtmain.lib QtNetwork4.dll QtNetwork4.lib QtOpenGL4.dll QtOpenGL4.lib \
            QtWebKit4.dll QtWebKit4.lib"

        for lib in $qtwebkit_libs_release ; do
            cp "$QTWEBKIT_DIR/lib/$lib" "$RELEASE_OUT_DIR"
        done

        local qtwebkit_imageplugins_debug="qgifd4.dll qicod4.dll qjpegd4.dll \
            qmngd4.dll qsvgd4.dll qtiffd4.dll"

        mkdir -p "$DEBUG_OUT_DIR/imageformats"
        for plugin in $qtwebkit_imageplugins_debug ; do
            cp "$QTWEBKIT_DIR/plugins/imageformats/$plugin" "$DEBUG_OUT_DIR/imageformats"
        done

        local qtwebkit_imageplugins_release="qgif4.dll qico4.dll qjpeg4.dll \
            qmng4.dll qsvg4.dll qtiff4.dll"

        mkdir -p "$RELEASE_OUT_DIR/imageformats"
        for plugin in $qtwebkit_imageplugins_release ; do
            cp "$QTWEBKIT_DIR/plugins/imageformats/$plugin" "$RELEASE_OUT_DIR/imageformats"
        done

        cp "$LLQTWEBKIT_DIR/debug/llqtwebkitd.lib"  "$DEBUG_OUT_DIR"
        cp "$LLQTWEBKIT_DIR/release/llqtwebkit.lib" "$RELEASE_OUT_DIR"

        cp "$LLQTWEBKIT_DIR/llqtwebkit.h" "$GLOBAL_INCLUDE_OUT_DIR/"

        # *TODO copy license files to $LICENSE_OUT_DIR/qt-4.5-linden-changes.txt, qt-4.5-LICENSE.LGPL, qt-4.5-LGPL_EXCEPTION.txt

    ;;
    "darwin")
        build="$(pwd)/build-darwin-i386"
        packages="$build/packages"
        install="$build/install"
        ./configure --prefix="$install"
        make
        make install
    ;;
    "linux")
        build="$(pwd)/build-linux-i686-relwithdebinfo"
        packages="$build/packages"
        install="$build/install"
        pushd "$QT_SOURCE_DIR"
            echo "DISTCC_HOSTS=$DISTCC_HOSTS"
            yes | head -n 1 | \
            MAKEFLAGS="-j12" CXX="distcc g++-4.1" CXXFLAGS="-DQT_NO_INOTIFY -m32 -fno-stack-protector" \
                             CC='distcc gcc-4.1' CFLAGS="-m32 -fno-stack-protector" \
                ./configure \
                -v -platform linux-g++-32  -fontconfig -fast -no-qt3support -static -release  -no-xmlpatterns -no-phonon \
                -openssl-linked -no-3dnow -no-sse -no-sse2 -no-gtkstyle -no-xinput -no-sm -buildkey LL$(date +%s) \
                -no-sql-sqlite -no-scripttools -no-cups -no-dbus -qt-libmng -no-glib -qt-libpng -opengl desktop  -no-xkb \
                -xrender -svg -no-pch -opensource -I"$packages/include" -L"$packages/lib" --prefix="$install/"
            make -j12
            export QTDIR=$(pwd)
            export PATH="$PATH:$QTDIR/bin"
            make install
        popd
        qmake -platform linux-g++-32 CONFIG-=debug
        make -j12

        mkdir -p "install/lib"
        cp "libllqtwebkit.a" "install/lib"

        mkdir -p "install/include"
        cp "llqtwebkit.h" "install/include"

        cp "$install/lib"/libQt*.a "install/lib"
        cp "$install/plugins/imageformats"/libq*.a "install/lib"
    ;;
esac
mkdir -p "install/LICENSES"
cp "LLQTWEBKIT_LICENSE.txt" "install/LICENSES/"

pass

