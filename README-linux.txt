This comes in three parts:
1) Build a 'special' Qt for use with llqtwebkit
2) Build llqtwebkit
3) Get the resulting bits in the SL Viewer (if that's what you're interested in)


1) BUILDING Qt FOR LINUX LLQTWEBKIT
-----------------------------------

NOTE: It is not strictly necessary to build a 'special' Qt - you can
use the system Qt if you already have it - though we strongly recommend
Qt version 4.5.2 or above, with our enhancement patches incorporated.
This section describes how we (Linden Lab) generate our distro-agnostic
static Qt libs specifically tailored for LLQtWebKit, including our own patches.

* On a ubuntu dapper (6.06) machine:
$ tar -zxf qt-all-opensource-src-4.5.2.tar.gz
$ cd qt-all-opensource-src-4.5.2

* we now apply a series of patches to Qt to enhance it for our purposes (better history handling, better frame handling, etc.)
* in numerical order, apply all the patches from the 'qt_patches' directory of llqtwebkit:
** for each FILE:
$ patch -p1 < FILE

* configure the build
$ CXXFLAGS='-DQT_NO_INOTIFY' ./configure -v -fontconfig -fast -no-qt3support -static -release  -no-xmlpatterns -no-phonon -openssl-linked -no-3dnow -no-sse -no-sse2 -no-gtkstyle -no-xinput -no-sm -buildkey LL`date +%s` -no-sql-sqlite -no-scripttools -no-cups -no-dbus -no-libmng -no-glib -qt-libpng -opengl desktop  -no-xkb -xrender -svg

* Accept the license, if you do.

* Wait a few minutes while it churns and bootstraps.

* Build Qt:
$ make

* Wait ages for it to build.

* You're done, but if you want to go on to build llqtwebkit on this machine, you'd really better do...

$ sudo make install


2) BUILDING LLQTWEBKIT
----------------------

* set up environment vars
$ export QTDIR=/usr/local/Trolltech/Qt-4.5.2
$ export PATH=$PATH:$QTDIR/bin

* configure llqtwebkit
$ qmake CONFIG-=debug

* build it
$ make


3) THE BITS THAT THE VIEWER BUILD NEEDS
---------------------------------------

* From Qt:
 lib/*.a
 plugins/imageformats/*.a

* From LLQtWebKit:
 libllqtwebkit.a
 llqtwebkit.h

The .a files go in {VIEWERSRC}/libraries/i686-linux/lib_release_client/
The .h files go in {VIEWERSRC}/libraries/include/

