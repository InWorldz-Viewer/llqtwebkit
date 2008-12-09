TEMPLATE = lib
CONFIG += static
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

!mac {
unix {
    DEFINES += LL_LINUX
}
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

RCC_DIR     = $$PWD/.rcc
UI_DIR      = $$PWD/.ui
MOC_DIR     = $$PWD/.moc
OBJECTS_DIR = $$PWD/.obj
