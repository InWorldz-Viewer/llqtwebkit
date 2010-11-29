@echo.
@echo This is a very simple batch file that makes debug and release
@echo versions of LLQtWebKit and then makes release versions of 
@echo testGL, uBrowser, QtTestApp and Snap Dragon.
@echo.
@echo To make the Win32GL test, run the copy_llqtwebkit.bat batch file after
@echo running this one and load the MSVC solution file and build as normal.
@echo.
@echo This probably won't work unless you run it from a Qt 4.7.x command
@echo prompt since it needs a path to the Qt 4.7.x build directory.
@echo.
@echo About to delete intermediate files - edit this file if that makes you sad.
@echo.
@pause

@rem Uncomment the next line if you DO NOT want to erase intermediate files first
@rem goto NO_ERASE

@rem Delete LLQtWebKit files
@rmdir .moc\ /s /q
@rmdir .obj\ /s /q
@rmdir .ui\ /s /q
@del Makefile
@del Makefile.Debug
@del Makefile.Release
@rmdir debug\ /s /q
@rmdir release\ /s /q

@rem Delete GL, GLUT and GLUI files
@rmdir tests\GL\ /s /q

@rem Delete QtTestApp files
@rmdir tests\qttestapp\Debug\ /s /q
@rmdir tests\qttestapp\Release\ /s /q
@del tests\qttestapp\Makefile
@del tests\qttestapp\Makefile.Debug
@del tests\qttestapp\Makefile.Release
@del tests\qttestapp\ui_window.h

@rem Delete testGL files
@rmdir tests\testgl\Debug\ /s /q
@rmdir tests\testgl\Release\ /s /q
@del tests\testgl\Makefile
@del tests\testgl\Makefile.Debug
@del tests\testgl\Makefile.Release

@rem Delete uBrowserfiles
@rmdir tests\ubrowser\Debug\ /s /q
@rmdir tests\ubrowser\Release\ /s /q
@del tests\ubrowser\Makefile
@del tests\ubrowser\Makefile.Debug
@del tests\ubrowser\Makefile.Release

@rem Delete snapdragon
@rmdir tests\snapdragon\Debug\ /s /q
@rmdir tests\snapdragon\Release\ /s /q
@del tests\snapdragon\Makefile
@del tests\snapdragon\Makefile.Debug
@del tests\snapdragon\Makefile.Release

@echo.
@echo Deleted intermediate files.
@echo.
@pause

:NO_ERASE

@rem location of GLUT and GLUI components we built previously
set GL_COMPONENT_DIR=C:\Work\qt\GL
xcopy %GL_COMPONENT_DIR%\*.* tests\GL\ /y

@rem clean and make a debug version of LLQtWebKit
@rem No longer patching Qt as of v4.7.0 so switch off code that referenced changes
qmake CONFIG+=debug DEFINES+=VANILLA_QT
nmake clean
nmake

@rem clean and make a release version of LLQtWebKit
@rem No longer patching Qt as of v4.7.0 so switch off code that referenced changes
qmake CONFIG-=debug DEFINES+=VANILLA_QT
nmake clean
nmake

@rem clean and make a release version of testGL test app
pushd .
cd tests\testgl
qmake CONFIG-=debug
nmake clean
nmake
popd

@rem clean and make a release version of QtTestApp test app
pushd .
cd tests\qttestapp
qmake CONFIG-=debug
nmake clean
nmake
popd

@rem clean and make a release version of uBrowser test app
pushd .
cd tests\ubrowser
qmake CONFIG-=debug
nmake clean
nmake
popd

@rem clean and make a release version of uBrowser test app
pushd .
cd tests\snapdragon
qmake CONFIG-=debug CONFIG+=console
nmake clean
nmake
popd

@rem Hard to see if builds fail so look for what we need afterwards
@if not exist debug\llqtwebkitd.lib echo ****** ERROR: Failed to build LLQtWebKit (debug) library
@if not exist release\llqtwebkit.lib echo ****** ERROR: Failed to build LLQtWebKit (release) library

@if not exist tests\gl\qttestapp.exe echo ****** ERROR: Failed to build QtTestApp test app
@if not exist tests\gl\snapdragon.exe echo ****** ERROR: Failed to build SnapDragon test app
@if not exist tests\gl\testgl.exe echo ****** ERROR: Failed to build testGL test app
@if not exist tests\gl\ubrowser.exe echo ****** ERROR: Failed to build uBrowser test app

@echo -- End of batch file --
