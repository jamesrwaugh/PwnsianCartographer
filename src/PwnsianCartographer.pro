TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++14
unix|win32: LIBS += -lnbt -ljson11 -lz -lSDL2 -lzip -lpthread

SOURCES += main.cpp \
    AnvilFile.cpp \
    utility.cpp \
    draw.cpp \
    AnvilFileWorld.cpp \
    blocks.cpp \
    lodepng.cpp \
    ChunkInterface.cpp \
    config.cpp \
    keyvalues.cpp

HEADERS += \
    AnvilFile.h \
    types.h \
    utility.h \
    draw.h \
    AnvilFileWorld.h \
    win32_dirent.h \
    blocks.h \
    ChunkInterface.h \
    config.h \
    keyvalues.h

