@rem ===== source/destination dirs that may change depending where you build mozilla =====
@set SRC_DIR_OPT="c:\mozilla.1.8.0.9\objdir-opt-xulrunner-small"
@set SRC_DIR_DEBUG="c:\mozilla.1.8.0.9\objdir-debug-xulrunner-small"
@set DEST_DIR="libraries\i686-win32"

@rem =============== instructions ===============
@echo This batch file copies the files required to build the 
@echo Linden Lab Mozilla Library (LLMozLib).
@echo.
@echo It will copy the required files from a Mozilla build in the specified 
@echo directories to the right location for LLMozLib.
@echo.
@echo The release build will be copied from:
@echo  %SRC_DIR_OPT%
@echo.
@echo and the debug build from
@echo   %SRC_DIR_DEBUG%
@echo.
@echo to the %DEST_DIR% directory under the current directory.
@echo.
@echo IMPORTANT: delete the contents of the destination directory first.
@echo.
@pause

@echo ========== copying header files ==========
xcopy %SRC_DIR_OPT%\dist\include\content\*.*					%DEST_DIR%\include\mozilla\include\content\ /s
xcopy %SRC_DIR_OPT%\dist\include\docshell\*.*					%DEST_DIR%\include\mozilla\include\docshell\ /s
xcopy %SRC_DIR_OPT%\dist\include\dom\*.*						%DEST_DIR%\include\mozilla\include\dom\ /s
xcopy %SRC_DIR_OPT%\dist\include\gfx\*.*						%DEST_DIR%\include\mozilla\include\gfx\ /s
xcopy %SRC_DIR_OPT%\dist\include\layout\*.*						%DEST_DIR%\include\mozilla\include\layout\ /s
xcopy %SRC_DIR_OPT%\dist\include\locale\*.*						%DEST_DIR%\include\mozilla\include\locale\ /s
xcopy %SRC_DIR_OPT%\dist\include\necko\*.*						%DEST_DIR%\include\mozilla\include\necko\ /s
xcopy %SRC_DIR_OPT%\dist\include\nkcache\*.*					%DEST_DIR%\include\mozilla\include\nkcache\ /s
xcopy %SRC_DIR_OPT%\dist\include\pref\*.*						%DEST_DIR%\include\mozilla\include\pref\ /s
xcopy %SRC_DIR_OPT%\dist\include\profdirserviceprovider\*.*		%DEST_DIR%\include\mozilla\include\profdirserviceprovider\ /s
xcopy %SRC_DIR_OPT%\dist\include\string\*.*						%DEST_DIR%\include\mozilla\include\string\ /s
xcopy %SRC_DIR_OPT%\dist\include\uriloader\*.*					%DEST_DIR%\include\mozilla\include\uriloader\ /s
xcopy %SRC_DIR_OPT%\dist\include\view\*.*						%DEST_DIR%\include\mozilla\include\view\ /s
xcopy %SRC_DIR_OPT%\dist\include\webbrwsr\*.*					%DEST_DIR%\include\mozilla\include\webbrwsr\ /s
xcopy %SRC_DIR_OPT%\dist\include\widget\*.*						%DEST_DIR%\include\mozilla\include\widget\ /s
xcopy %SRC_DIR_OPT%\dist\include\xpcom\*.*						%DEST_DIR%\include\mozilla\include\xpcom\ /s
xcopy %SRC_DIR_OPT%\dist\include\xulapp\*.*						%DEST_DIR%\include\mozilla\include\xulapp\ /s
xcopy %SRC_DIR_OPT%\dist\sdk\include\*.*						%DEST_DIR%\include\mozilla\sdk\include\	/s

@echo ========== copying debug libraries ==========
xcopy %SRC_DIR_DEBUG%\dist\lib\nspr4.lib						%DEST_DIR%\lib_debug\
xcopy %SRC_DIR_DEBUG%\dist\lib\plc4.lib							%DEST_DIR%\lib_debug\
xcopy %SRC_DIR_DEBUG%\dist\lib\profdirserviceprovider_s.lib		%DEST_DIR%\lib_debug\
xcopy %SRC_DIR_DEBUG%\dist\lib\xpcom.lib						%DEST_DIR%\lib_debug\
xcopy %SRC_DIR_DEBUG%\dist\lib\xul.lib							%DEST_DIR%\lib_debug\

@echo ========== copying debug runtime files ==========
xcopy %SRC_DIR_DEBUG%\dist\bin\chrome\*.*						%DEST_DIR%\runtime_debug\chrome\ /s
xcopy %SRC_DIR_DEBUG%\dist\bin\components\*.*					%DEST_DIR%\runtime_debug\components\ /s
xcopy %SRC_DIR_DEBUG%\dist\bin\greprefs\*.*						%DEST_DIR%\runtime_debug\greprefs\ /s
xcopy %SRC_DIR_DEBUG%\dist\bin\plugins\*.*						%DEST_DIR%\runtime_debug\plugins\ /s
xcopy %SRC_DIR_DEBUG%\dist\bin\res\*.*							%DEST_DIR%\runtime_debug\res\ /s
xcopy %SRC_DIR_DEBUG%\dist\bin\*.dll							%DEST_DIR%\runtime_debug\
xcopy %SRC_DIR_DEBUG%\dist\bin\windbgdlg.exe					%DEST_DIR%\runtime_debug\

@echo ========== copying release (opt) libraries ==========
xcopy %SRC_DIR_OPT%\dist\lib\nspr4.lib							%DEST_DIR%\lib_release\
xcopy %SRC_DIR_OPT%\dist\lib\plc4.lib							%DEST_DIR%\lib_release\
xcopy %SRC_DIR_OPT%\dist\lib\profdirserviceprovider_s.lib		%DEST_DIR%\lib_release\
xcopy %SRC_DIR_OPT%\dist\lib\xpcom.lib							%DEST_DIR%\lib_release\
xcopy %SRC_DIR_OPT%\dist\lib\xul.lib							%DEST_DIR%\lib_release\

@echo ========== copying release (opt) runtime files ==========
xcopy %SRC_DIR_OPT%\dist\bin\chrome\*.*							%DEST_DIR%\runtime_release\chrome\ /s
xcopy %SRC_DIR_OPT%\dist\bin\components\*.*						%DEST_DIR%\runtime_release\components\ /s
xcopy %SRC_DIR_OPT%\dist\bin\greprefs\*.*						%DEST_DIR%\runtime_release\greprefs\ /s
xcopy %SRC_DIR_OPT%\dist\bin\plugins\*.*						%DEST_DIR%\runtime_release\plugins\ /s
xcopy %SRC_DIR_OPT%\dist\bin\res\*.*							%DEST_DIR%\runtime_release\res\ /s
xcopy %SRC_DIR_OPT%\dist\bin\*.dll								%DEST_DIR%\runtime_release\

@echo ========== finished ==========
@pause