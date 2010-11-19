Build instructions can be found here:

https://wiki.secondlife.com/wiki/LLQtWebKit

To build Qt 4.7.1, use the following configure line in place of the one on the page above:

./configure -platform macx-g++40 -no-framework -fast -no-qt3support -prefix $PWD -static -release -no-xmlpatterns -no-phonon -webkit -sdk /Developer/SDKs/MacOSX10.5.sdk/ -cocoa -nomake examples -nomake demos -nomake docs -nomake translations 

This builds against the 10.5 SDK and tells Qt to use its Cocoa rendering path.
