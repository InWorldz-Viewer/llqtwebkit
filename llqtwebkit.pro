CONFIG += static
TEMPLATE = lib
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

include(llqtwebkit.pri)

QT += webkit opengl network gui

win32:debug {
  TARGET = llqtwebkitd
}

RCC_DIR     = $$PWD/.rcc
UI_DIR      = $$PWD/.ui
MOC_DIR     = $$PWD/.moc
OBJECTS_DIR = $$PWD/.obj
