TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++14
unix|win32: LIBS += -lnbt -ljson11 -lz -lSDL2 -lzip -lpthread -lpng

SOURCES += main.cpp \
    utility.cpp \
    draw.cpp \
    blocks.cpp \
    lodepng.cpp \
    ChunkInterface.cpp \
    config.cpp \
    keyvalues.cpp \
    savepng.c \
    RegionFile.cpp \
    RegionFileWorld.cpp

HEADERS += \
    types.h \
    utility.h \
    draw.h \
    win32_dirent.h \
    blocks.h \
    ChunkInterface.h \
    config.h \
    keyvalues.h \
    savepng.h \
    lodepng.h \
    RegionFile.h \
    RegionFileWorld.h

