@rem ===== source/destination dirs that may change =====
@set QT_SRC_DIR=C:\qt\qt-all-opensource-src-4.5.2
@set OPENSSL_SRC_DIR=F:\openssl-0.9.8k
@set LLQTWEBKIT_SRC_DIR=F:\checkouts\llqtwebkit
@set SL_DEST_DIR="C:\Documents and Settings\callum\Desktop"

@rem =============== instructions ===============
@echo This batch file copies the Qt, OpenSSL and LLQtWebKit files required 
@echo by the Second Life client into a SL client source tree.
@echo.
@pause

@rem =============== Qt core libs and dlls for debug and release ===============
xcopy %QT_SRC_DIR%\lib\QtCore4.dll     %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtCore4.lib     %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtCored4.dll    %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtCored4.lib    %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtGui4.dll      %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtGui4.lib      %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtGuid4.dll     %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtGuid4.lib     %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtNetwork4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtNetwork4.lib  %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtNetworkd4.dll %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtNetworkd4.lib %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtOpenGL4.dll   %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtOpenGL4.lib   %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtOpenGLd4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtOpenGLd4.lib  %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtWebKit4.dll   %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtWebKit4.lib   %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\QtWebKitd4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\QtWebKitd4.lib  %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %QT_SRC_DIR%\lib\qtmain.lib      %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %QT_SRC_DIR%\lib\qtmaind.lib     %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y

@rem =============== Qt plugin dlls dlls for debug and release ===============
xcopy %QT_SRC_DIR%\plugins\imageformats\qgifd4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qicod4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qjpegd4.dll %SL_DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qmngd4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qsvgd4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qtiffd4.dll %SL_DEST_DIR%\libraries\i686-win32\lib\debug\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qgif4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qico4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qjpeg4.dll %SL_DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qmng4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qsvg4.dll  %SL_DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y
xcopy %QT_SRC_DIR%\plugins\imageformats\qtiff4.dll %SL_DEST_DIR%\libraries\i686-win32\lib\release\imageformats\ /y

@rem =============== OpenSSL dlls ===============
xcopy %OPENSSL_SRC_DIR%\out32dll\ssleay32.dll %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %OPENSSL_SRC_DIR%\out32dll\libeay32.dll %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %OPENSSL_SRC_DIR%\out32dll\ssleay32.dll %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y
xcopy %OPENSSL_SRC_DIR%\out32dll\libeay32.dll %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y

@rem =============== LLQtWebKit header and libs for debug and release ===============
xcopy %LLQTWEBKIT_SRC_DIR%\llqtwebkit.h           %SL_DEST_DIR%\libraries\include\ /y
xcopy %LLQTWEBKIT_SRC_DIR%\debug\llqtwebkitd.lib  %SL_DEST_DIR%\libraries\i686-win32\lib\debug\ /y
xcopy %LLQTWEBKIT_SRC_DIR%\release\llqtwebkit.lib %SL_DEST_DIR%\libraries\i686-win32\lib\release\ /y

@echo -- Complete --
@pause
