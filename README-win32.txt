Getting, patching, building and using LLMozLib2 on the Windows platform
=======================================================================

These instructions describe how to build the Windows version of LLMozLib2 for use within the Second Life client. They have been tested using Microsoft Visual Studio 2003 (v7.1) - other versions of the Microsoft compiler may also work but they have not been tried or tested.

LLMozLib2 currently uses code from the 1.8.1.x branch of the Mozilla codebase - somewhat similar to what is used in Firefox 2.0.

Here are the steps you need to take to set up your build environment, get the Mozilla source, patch it, build it and copy it to the right place in the LLMozLib2 source tree. After that, you can build LLMozLib2 normally using the Visual Studio solution file.

* Checkout llmozlib2 from the Second Life public SVN repository: https://svn.secondlife.com/svn/llmozlib/trunk/llmozlib2/ 

* Follow the instructions on the Mozilla Windows Build Prerequisites page - http://developer.mozilla.org/en/docs/Windows_Build_Prerequisites

* Typically this consists of downloading and installing the MozillaBuild package into C:\mozilla-build - refer to the Web page for details since it's likely this will change over time and that page will obviously always have the most recent version.

* Get to an MSYS command prompt as per instructions described in the aforementioned Web page.

* Change directory to the MSYS /c drive (the C: drive in Windows) 

   cd /c 

* Checkout the Mozilla configuration file 

    export CVSROOT=':pserver:anonymous@cvs-mirror.mozilla.org:/cvsroot'
    cvs checkout -r FIREFOX_2_0_0_13_RELEASE mozilla/client.mk 
    
    Notes:  * replace the FIREFOX_2_0_0_13_RELEASE tag with one that refers to the version you want
            * the CVS password is 'anonymous'
    
* Change to the directory you checked out in 

    cd mozilla 

* Checkout the source code 

    make -f client.mk checkout MOZ_CO_PROJECT=xulrunner 

* Copy the following files from the llmozlib2/build_mozilla directory into the C:\mozilla directory you just checked out 

    * .mozconfig.debug
    * .mozconfig.optimized
    * linden.patch 

* Patch the Mozilla source with the Linden patch

    patch -p0 < linden.patch 

* Copy over the .mozconfig file for the optimized (release) build 

    cp .mozconfig.optimized .mozconfig 

* Start a build: 

    make -f client.mk build 

* Wait - takes about 20 minutes on a typical development system

* Copy over the .mozconfig file for the debug build 

    cp .mozconfig.debug .mozconfig 

* Start a build: 

    make -f client.mk build 

* Wait - takes about 40 minutes on a typical development system 

* Unify the Mozilla runtime type libraries - convert the *.xpt files into a single, unified one - improves startup time considerably. 

    * Using the MSYS shell from the /c/mozilla/objdir-opt-xulrunner-small/dist/bin directory:

        ./xpt_link all.tmp components/*.xpt 
        rm components/*.xpt 
        mv all.tmp components/all.xpt 

    * Using the MSYS shell from the /c/mozilla/objdir-debug-xulrunner-small/dist/bin directory:

        ./xpt_link all.tmp components/*.xpt 
        rm components/*.xpt 
        mv all.tmp components/all.xpt 

* Copy over the necessary Mozilla files into the LLMozLib2 directory 

    copy_products_windows.bat 
    
    Note: (run from the LLMozLib2 directory) 

* Build LLMozLib2 using Microsoft Visual Studio 2003 (v7.1) 

    Open the LLMozLib2 solution file llmozlib2.sln. 

* Build the LLMozLib Debug and Release configurations in the normal fashion. 

* Copy the LLMozLib2 files to the Second Life client branch 

    copy_llmozlib2_to_branch.bat 
    
    Note1: (run from the LLMozLib2 directory) 
    Note2: (edit the batch file to reflect the branch you want to copy to) 

* That completes the process - you should now have a new set of LLMozLib2/Mozilla runtime files in the branch you specified. The files that are copied are:
    * Header file (llmozlib2.h) in libraries\include\
    * Static library (llmozlib2.lib) (debug) in libraries\i686-win32\lib_debug\
    * Static library (llmozlib2d.lib) (release) in libraries\i686-win32\lib_release\
    * DLLs (debug) in libraries\i686-win32\lib_debug\
    * DLLs (release) in libraries\i686-win32\lib_release\
    * Runtime misc files (debug) in indra\newview\app_settings\mozilla_debug\
    * Runtime misc files (release) in indra\newview\app_settings\mozilla_release\

* If you want to experiment with the test applications (uBrowser, testGL for example), you can change the startup project in Visual Studio and build the one you want. A separate document in this directory contains notes on using each test app.

---- end of doc ----