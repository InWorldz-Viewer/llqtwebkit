TEMPLATE = lib
CONFIG += static
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

include(llmozlib2.pri)

QT += webkit opengl network

RCC_DIR     = $$PWD/.rcc
UI_DIR      = $$PWD/.ui
MOC_DIR     = $$PWD/.moc
OBJECTS_DIR = $$PWD/.obj
