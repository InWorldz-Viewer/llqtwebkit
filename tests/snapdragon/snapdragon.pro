TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../
CONFIG -= app_bundle

QT += webkit opengl network

!mac {
unix {
    DEFINES += LL_LINUX
    LIBS += $$PWD/../../libllqtwebkit.a
}
}

mac {
    DEFINES += LL_OSX
    LIBS += $$PWD/libllqtwebkit.dylib
}

win32{
    DEFINES += LL_WINDOWS
    DESTDIR=../GL
    LIBS += user32.lib 
    
    debug {
      LIBS += $$PWD/../../Debug/llqtwebkitd.lib
    }
    release {
      LIBS += $$PWD/../../Release/llqtwebkit.lib
    }
}

include(../../static.pri)

# Input
SOURCES += snapdragon.cpp
