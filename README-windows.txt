If you want or need to build the Mozilla codebase, refer to the page in the Linden Lab wiki here:

https://osiris.lindenlab.com/mediawiki/index.php/BuildingMozillaWindows

If you follow those instructions, you will end up with release(opt) and debug builds in the c:\mozilla directory.

The patch file as well as sample .mozconfig files can be found in the build_mozilla directory.

Run the batch file:

copy_products_windows.bat

and it will copy the files needed to build llMozLib into the "libraries\i686-win32" directory.

You can then open llmozlib.sln in MSVC 7.1 and build llMozLib.

There are some test applications in the "test" directory.