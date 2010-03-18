@echo.
@echo This is a very simple batch file that makes debug and release
@echo versions of LLQtWebKit and then makes release versions of 
@echo testGL, uBrowser and QtTestApp. 
@echo.
@echo To make the Win32GL test, run the copy_files.bat batch file after
@echo running this one and load the MSVC solution file and build as normal.
@echo.
@echo This probably won't work unless you run it from a Qt command prompt
@echo since it needs a path to the Qt build directory.
@echo.
@pause

@rem location of GLUT and GLUI components we built previously
set GL_COMPONENT_DIR=C:\Work\qt\GL
xcopy %GL_COMPONENT_DIR%\*.* tests\GL\ /y

@rem clean and make a debug version of LLQtWebKit
nmake clean
qmake CONFIG+=debug
nmake clean
nmake

@rem clean and make a release version of LLQtWebKit
nmake clean
qmake CONFIG-=debug
nmake clean
nmake

@rem clean and make a release version of testGL test app
pushd .
cd tests\testgl
nmake clean
qmake CONFIG-=debug
nmake clean
nmake
popd

@rem clean and make a release version of QtTestApp test app
pushd .
cd tests\qttestapp
nmake clean
qmake CONFIG-=debug
nmake clean
nmake
popd

@rem clean and make a release version of uBrowser test app
pushd .
cd tests\ubrowser
nmake clean
qmake CONFIG-=debug
nmake clean
nmake
popd

@rem switch to root of tests directory - some need to be run from their own dir
cd tests

pause