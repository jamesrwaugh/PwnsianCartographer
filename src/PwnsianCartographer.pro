TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++14
unix|win32: LIBS += -lnbt -ljson11 -lz -lSDL2 -lzip

SOURCES += main.cpp \
    AnvilFile.cpp \
    utility.cpp \
    draw.cpp \
    AnvilFileWorld.cpp \
    blocks.cpp \
    lodepng.cpp \
    ChunkInterface.cpp

HEADERS += \
    AnvilFile.h \
    types.h \
    utility.h \
    draw.h \
    AnvilFileWorld.h \
    win32_dirent.h \
    blocks.h \
    ChunkInterface.h

