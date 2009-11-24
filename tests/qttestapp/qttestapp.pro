TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../

#CONFIG += static

include(/Users/girish/webkit-git/WebKit.pri)

QT += opengl network

!mac {
unix {
    DEFINES += LL_LINUX
    LIBS += $$PWD/../../libllqtwebkit.a
}
}

mac {
    DEFINES += LL_OSX
    LIBS += $$PWD/../../libllqtwebkit.dylib
}


win32{
    DEFINES += _WINDOWS
    INCLUDEPATH += ../
    LIBS += -L../GL 
    DESTDIR=../GL
    debug {
      LIBS += $$PWD/../../Debug/llqtwebkitd.lib
    }
    release {
      LIBS += $$PWD/../../Release/llqtwebkit.lib
    }
}

# Input
SOURCES += main.cpp
FORMS += window.ui
