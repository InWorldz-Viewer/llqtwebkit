TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../

#CONFIG += static
QT += opengl network
include(/Users/girish/webkit-git/WebKit.pri)

!mac {
unix {
    INCLUDEPATH += $$system(pkg-config gtk+-2.0 --cflags | sed -e s/-I//g)
#    DEFINES += LL_LINUX
    DEFINES += LL_LINUX LL_NEWER_GLUI
    LIBS += -lglui -lglut
    LIBS += $$system(pkg-config gtk+-2.0 --libs)
	LIBS += $$PWD/../../libllmozlib2.a
}
}

mac {
    LIBS += -framework GLUT -framework OpenGL -framework GLUI
    DEFINES += LL_OSX
	LIBS += $$PWD/../../libllqtwebkit.dylib
    INCLUDEPATH += /Library/Frameworks/GLUI.framework/Headers
}


win32 {
    DEFINES += _WINDOWS
    INCLUDEPATH += ../ ../GL
    DESTDIR=../GL
    LIBS += -L../GL
    debug {
      LIBS += $$PWD/../../Debug/llqtwebkitd.lib
    }
    release {
      LIBS += $$PWD/../../Release/llqtwebkit.lib
    }
}

# Input
HEADERS += ubrowser.h
SOURCES += app.cpp ubrowser.cpp
