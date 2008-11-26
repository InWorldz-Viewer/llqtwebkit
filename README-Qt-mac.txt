1) Get Qt and set environment variables.

Download Qt from trolltech.com

Extract it somewhere such as
$HOME/dev/qt-4.4/

Then set the environment variable QTDIR to point to that location.
export QTDIR=$HOME/dev/qt-4.4
export PATH=$PATH:$QTDIR/bin

2) Compile Qt

cd $QTDIR
./configure -no-framework -fast -no-qt3support -prefix $PWD -static -release -no-xmlpatterns -no-phonon
make

3) Get llmozlib and checkout the qtwebkit branch

cd $HOME/dev
git://code.staikos.net/llmozlib
git checkout -b qtwebkit origin/qtwebkit

4) Build llmolib2

cd $HOME/dev/llmozlib/llmozlib2/
./build-linux-llmozlib.sh

5) Compile ubrowser
cd tests/ubrowser/
qmake
make
