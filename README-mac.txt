The projects and scripts in this directory create the llmozlib library and package it with the mozilla runtime.

If you just need to rebuild the llmozlib library, you can skip ahead to building the xcode project.

If you want to start from scratch, you should first build the mozilla project.  The script ../mozilla-mac-universal-build/checkout_patch_build.sh will check out the mozilla sources from the mozilla cvs server, apply our local patches, and build the whole thing.

Once you have the mozilla library built, run the copy_products_mac.sh script in this directory to pull the necessary bits across.  This will repopulate the architecture-specific portions of the libraries directory here, so you may want to do a preemptive 'svn remove' and commit first.  It will also create mozilla-universal-darwin.tgz which contains the necessary runtime bits that need to go into the application bundle.

Once you've got the mozilla libraries in place, open llmozlib.xcodeproj and build the "Release" build style.  If the build fails with a bunch of missing include files, close the project and reopen it, then try again.  (A script phase in the project creates a symlink, but xcode only reevaluates the wildcard header path search when the project is opened.  You should only have to do this once per checkout.)

After that completes, run repackage_runtime.sh to do the final packaging step and copy the necessary pieces into your sandbox.  The script takes a single argument, which is the location of the sandbox directory.  (Note that this should be the directory containing the 'libraries' and 'indra' directories.)  The script should be run from the llmozlib directory, and the sandbox path can either be relative or absolute (i.e. './repackage_runtime.sh ../../BRANCH' will work fine).

-----------
NOTE: about llmozlib.exp

In order to hide internal symbols in the mozilla library, we have to use an explicit export list when building llmozlib.

The list of symbols to export is in llmozlib.exp.  The symbols are in "name mangled" form, making them a bit of a pain to read.

If changes are made to any of the class definitions in llmozlib.h, this list may need to be regenerated.  To regenerate the list of symbols:

- Open up the xcode project
- Change the "Exported Symbols File" setting in the llmozlib target from llmozlib.exp to nothing
- Build the "Release" target
- Execute the following commands:

otool -vT build/Release/libllmozlib.dylib >symbols.txt
sed -n -e '/.*\.eh/d' -e 's/^single module    \(.*\)/\1/p' <symbols.txt >symbols2.txt

- symbols2.txt should now contain the list of global symbols that the library would export with no controls in place.

Now comes the hard part.  The list must be manually pruned.  At the moment, deleting all symbols that don't begin with __ZN8LLMozLib seems to be correct.  Look at the existing llmozlib.exp for guidance.

Once you've got the new list, paste it into llmozlib.exp and change the "Exported Symbols File" setting back to "llmozlib.exp".

-----------
NOTE: about llmozlib_stub.c

Due to the way the Mac linker works, it was necessary to create a 'stub' library.  This is a shared library containing nothing but the export symbols that is used to make the linker happy when building the application.  The real shared library then gets referenced during runtime.

There's a shell script phase in the project that runs a 'sed' script which creates llmozlib_stub.c from llmozlib.exp.  It's about as stupid as possible.  

Make sure the stub library gets copied into libraries/universal-darwin/lib_{debug|release} and that the real library gets put into the application bundle.  Getting either one into the other's place will cause problems.
