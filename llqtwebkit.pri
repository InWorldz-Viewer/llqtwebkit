DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

!mac {
unix {
    DEFINES += LL_LINUX
}
}

mac {
    DEFINES += LL_OSX
}

win32{
    DEFINES += _WINDOWS
}

# Input
HEADERS += llembeddedbrowser.h \
           llembeddedbrowser_p.h \
           llembeddedbrowserwindow.h \
           llembeddedbrowserwindow_p.h \
           llmozlib2.h \
           llwebpage.h

SOURCES += llembeddedbrowser.cpp \
           llembeddedbrowserwindow.cpp \
           llmozlib2.cpp \
           llwebpage.cpp

RCC_DIR     = .rcc
UI_DIR      = .ui
MOC_DIR     = .moc
OBJECTS_DIR = .obj

include(qtwebkit_cookiejar/src/src.pri)
