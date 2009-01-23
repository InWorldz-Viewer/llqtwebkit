Building the open source Qt snapshot with Visual Studio:

 Start/All Programs/Microsoft Visual Studio 2005/Visual Studio Tools/Visual Studio 2005 Command Prompt
 mkdir C:\Qt
 cd C:\Qt
 git clone git://labs.trolltech.com/qt-snapshot
 cd qt-snapshot
 configure -no-qt3support -prefix C:\Qt\qt-snapshot
 If you accept the license agreement type 'yes' and enter
 run 'nmake sub-src' which will build Qt, but not the demos.

 After building Qt the release and debug libaries will be in C:\Qt\qt-snapshot\lib

 Create the file C:\Qt\qt-snapshot\bin\qt-vars.bat and put the following in it

@echo off
echo Setting up a Qt environment...
set QTDIR=C:\Qt\qt-snapshot
set PATH=C:\Qt\qt-snapshot\bin;%PATH%
set QMAKESPEC=win32-msvc2005
#call "C:\Program Files\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat"

Copy a Command prompt shortcut and modify the target and start in
Target: %COMSPEC% /k "C:\Qt\qt-snapshto\bin\qt-vars.bat"
Start in: C:\Qt\qt-snapshot

Rename the command prompt shortcut to "Qt Snapshot Command Prompt.

You can test it by opening the prompt and typeing qmake --version.




0) Acquire llmozlib
 a. Download and install Git windows package from http://code.google.com/p/msysgit/
 b. From a console run 'git clone git://code.staikos.net/llmozlib'
 c. Checkout the qtwebkit branch by doing 'git checkout -b qtwebkit origin/qtwebkit'

1) Acquire dependencies
 a. Qt: 4.4.x or newer, install the commercial binary package.
 b. Glut: http://www.xmission.com/~nate/glut.html  Grab the library and header zip.
 b1. Rename the downloaded directory to C:\llmozlib\llmozlib2\tests\GL
 c. Glui32: http://glui.sourceforge.net/
 c1. Extract the zip and follow the instructions to compile the library
     Make sure to build a debug library
 c2. Copy glui.h and from the include into C:\llmozlib\llmozlib2\tests\GL
 c3. Copy glui32.lib from msvc/lib into C:\llmozlib\llmozlib2\tests\GL

2) Build llmozlib.a
 a. Launch the Qt command prompt
'
 cd C:\llmozlib\llmozlib2
 qmake
 make
'

3) Build testgl and run it
 cd C:\llmozlib\llmozlib2\tests\testgl
 qmake CONFIG-=debug
 nmake
 ..\GL\testgl.exe

4) Build ubrowser and run it
 cd C:\llmozlib\llmozlib2\tests\ubrowser
 qmake CONFIG-=debug
 nmake
 ..\GL\ubrowser.exe

Notes:
a.To use Visual studio 2005, under qt command prompt to get a .vcproj, run
  qmake -tp vc

b. Make sure all libs and app are built as Multi-threaded Dll or Multi-threaded debug dll.
 In Visual Studio this is in Properties->configuration->c/c++->code generation->runtime library

c. Make sure the Qt.dll's are in the PATH or current directory
