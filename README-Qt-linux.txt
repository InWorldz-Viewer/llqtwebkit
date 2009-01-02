1) Get Qt and set environment variables.

Download Qt from trolltech.com

Extract it somewhere such as
$HOME/dev/qt-4.4/

Then set the environment variable QTDIR to point to that location.
export QTDIR=$HOME/dev/qt-4.4
export PATH=$PATH:$QTDIR/bin

2) Compile Qt

cd $QTDIR
./configure -no-framework -fast -no-qt3support -prefix $PWD -static -release -no-xmlpatterns -no-phonon -universal -sdk /Developer/SDKs/MacOSX10.4u.sdk/
make

3) Get llmozlib and checkout the qtwebkit branch

cd $HOME/dev
git://code.staikos.net/llmozlib
git checkout -b qtwebkit origin/qtwebkit

4) build llmozlib2 with qmake

cd $HOME/dev/llmozlib/llmozlib2/
qmake CONFIG-=debug
make

5) build ubrowser with qmake:

cd tests/ubrowser
qmake CONFIG-=debug
make

6) Run

./ubrowser

