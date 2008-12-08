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

4)  build llmozlib2.dylib with XCode:

cd $HOME/dev/llmozlib/llmozlib2/

Create a symlink to QTDIR in the same directory as llmozlib2.xcodeproj (this allows the xcode project to find it easily):

ln -s $QTDIR QTDIR

build the "Release" configuration of the "llmozlib2" target:

xcodebuild -project llmozlib2.xcodeproj -target llmozlib2 -configuration Release

5) build ubrowser with xcode:

cd tests/ubrowser
xcodebuild -project ubrowser.xcodeproj -target ubrowser -configuration Release
open build/Release/ubrowser.app


