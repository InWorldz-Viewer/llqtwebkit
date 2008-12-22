TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += qtestlib
QT += webkit opengl

include(../../llmozlib2.pri)
DEFINES += AUTOTEST

# Input
SOURCES += tst_llembeddedbrowserwindow.cpp

