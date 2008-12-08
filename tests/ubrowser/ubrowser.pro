TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../

CONFIG += static

QT += webkit opengl
LIBS += $$PWD/../../libllmozlib2.a

!mac && unix {
    INCLUDEPATH += $$system(pkg-config gtk+-2.0 --cflags | sed -e s/-I//g)
    DEFINES += LL_LINUX LL_NEWER_GLUI
    LIBS += -lglui
    LIBS += $$system(pkg-config gtk+-2.0 --libs)
}

mac {
    LIBS += -framework GLUT -framework OpenGL -framework GLUI
    DEFINES += LL_OSX
    QTPLUGIN += qgif
}

# Input
HEADERS += ubrowser.h
SOURCES += app.cpp ubrowser.cpp
