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
        build_sln "contrib/vstudio/vc8/zlibvc.sln" "Debug|Win32"
        build_sln "contrib/vstudio/vc8/zlibvc.sln" "Release|Win32"
        mkdir -p install/lib/debug
        mkdir -p install/lib/release
        cp "contrib/vstudio/vc8/x86/ZlibStatDebug/zlibstat.lib" \
            "install/lib/debug/zlibd.lib"
        cp "contrib/vstudio/vc8/x86/ZlibStatRelease/zlibstat.lib" \
            "install/lib/release/zlib.lib"
        mkdir -p "install/include/zlib"
        cp {zlib.h,zconf.h} "install/include/zlib"
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

