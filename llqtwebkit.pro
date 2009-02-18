CONFIG += static
TEMPLATE = lib
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

include(llmozlib2.pri)

QT += webkit opengl network gui

win32:debug {
  TARGET = llmozlib2d
}

RCC_DIR     = $$PWD/.rcc
UI_DIR      = $$PWD/.ui
MOC_DIR     = $$PWD/.moc
OBJECTS_DIR = $$PWD/.obj
