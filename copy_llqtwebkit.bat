@rem ===== source/destination dirs that may change =====
@set QT_SRC_DIR=C:\qt\qt-all-opensource-src-4.5.2
@set LLQTWEBKIT_SRC_DIR=C:\llqtwebkit
@set DEST_DIR="C:\Documents and Settings\callum\Desktop"

@rem =============== instructions ===============
@echo This batch file copies the Qt, OpenSSL and LLQtWebKit files required 
@echo by the Second Life client into a SL client source tree.
@echo.
@pause

@rem =============== Qt core libs and dlls for debug and release ===============
xcopy %QT_SRC_DIR%\lib\QtCore4.dll     %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtCore4.lib     %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtCored4.dll    %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtCored4.lib    %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtGui4.dll      %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtGui4.lib      %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtGuid4.dll     %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtGuid4.lib     %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtNetwork4.dll  %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtNetwork4.lib  %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtNetworkd4.dll %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtNetworkd4.lib %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtOpenGL4.dll   %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtOpenGL4.lib   %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtOpenGLd4.dll  %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtOpenGLd4.lib  %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtWebKit4.dll   %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtWebKit4.lib   %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtWebKitd4.dll  %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtWebKitd4.lib  %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\qtmain.lib      %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\qtmaind.lib     %DEST_DIR%\libraries\i686-win32\lib\debug\ /y

@rem =============== Qt plugin dlls dlls for debug and release ===============
xcopy %QT_SRC_DIR%\plugins\imageformats\qgifd4.dll  %DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qicod4.dll  %DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qjpegd4.dll %DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qmngd4.dll  %DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qsvgd4.dll  %DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qtiffd4.dll %DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qgif4.dll  %DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qico4.dll  %DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qjpeg4.dll %DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qmng4.dll  %DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qsvg4.dll  %DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qtiff4.dll %DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y

@rem =============== OpenSSL dlls ===============
xcopy %QT_SRC_DIR%\bin\ssleay32.dll %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\bin\libeay32.dll %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\bin\ssleay32.dll %DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\bin\libeay32.dll %DEST_DIR%\libraries\i686-win32\lib\release\ /y

@rem =============== LLQtWebKit header and libs for debug and release ===============
xcopy %LLQTWEBKIT_SRC_DIR%\llqtwebkit.h           %DEST_DIR%\libraries\include\ /y
xcopy %LLQTWEBKIT_SRC_DIR%\debug\llqtwebkitd.lib  %DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %LLQTWEBKIT_SRC_DIR%\release\llqtwebkit.lib %DEST_DIR%\libraries\i686-win32\lib\release\ /y

@echo -- Complete --
@pause
