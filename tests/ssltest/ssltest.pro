TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../
CONFIG -= app_bundle
CONFIG += console

QT += webkit network

mac {
    DEFINES += LL_OSX
    LIBS += $$PWD/libllqtwebkit.dylib
}

win32 {
    DEFINES += _WINDOWS
    INCLUDEPATH += ../
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

SOURCES += ssltest.cpp
