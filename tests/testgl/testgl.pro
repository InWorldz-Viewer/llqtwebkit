TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../

QT += webkit opengl network

!mac {
unix {
    DEFINES += LL_LINUX
#    DEFINES += LL_LINUX LL_NEWER_GLUI
    LIBS += -lglui -lglut
    LIBS += $$PWD/../../libllmozlib2.a
}
}

mac {
    DEFINES += LL_OSX
    LIBS += -framework GLUT -framework OpenGL
}


win32{
    DEFINES += _WINDOWS
    INCLUDEPATH += ../
    LIBS += -L../GL 
    DESTDIR=../GL
    debug {
      LIBS += $$PWD/../../Debug/llmozlib2d.lib
    }
    release {
      LIBS += $$PWD/../../Release/llmozlib2.lib
    }
}

include(../../static.pri)

# Input
SOURCES += testgl.cpp
