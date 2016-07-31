TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
unix|win32: LIBS += -lnbt -ljson11 -lSDL2 -lzip

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -std=c++14 -O4
} else {
    QMAKE_CXXFLAGS += -std=c++14
}

unix {
    LIBS += -lpng -lpthread
}

HEADERS += \
    config.h \
    types.h \
    utility.h \
    blocks/blocks.h \
    anvil/ChunkInterface.h \
    anvil/RegionFile.h \
    anvil/RegionFileWorld.h \
    draw/draw.h \
    extlibs/keyvalues.h \
    extlibs/lodepng.h \
    extlibs/savepng.h \
    extlibs/win32_dirent.h

SOURCES += \
    config.cpp \
    main.cpp \
    utility.cpp \
    blocks/blocks.cpp \
    anvil/ChunkInterface.cpp \
    anvil/RegionFile.cpp \
    anvil/RegionFileWorld.cpp \
    draw/draw.cpp \
    extlibs/keyvalues.cpp \
    extlibs/lodepng.cpp \
    extlibs/savepng.c

