TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../

CONFIG += static
QT += webkit opengl network

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
    QTPLUGIN += qgif
}


win32 {
    DEFINES += _WINDOWS
    INCLUDEPATH += ../ ../GL
    LIBS += -L../GL $$PWD/../../Release/llmozlib2.lib
    DESTDIR=../GL
}

# Input
HEADERS += ubrowser.h
SOURCES += app.cpp ubrowser.cpp
