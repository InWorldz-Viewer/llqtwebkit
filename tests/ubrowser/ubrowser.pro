######################################################################
# Automatically generated by qmake (2.01a) Fri Nov 21 13:25:29 2008
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../

CONFIG += static

QT += webkit opengl

DEFINES += LL_OSX
//QMAKE_LFLAGS += -L/path/to/framework/directory/
LIBS += -framework GLUT -framework OpenGL -framework GLUI $$PWD/../../libllmozlib2.a
QTPLUGIN += qgif
# Input
HEADERS += ubrowser.h
SOURCES += app.cpp ubrowser.cpp