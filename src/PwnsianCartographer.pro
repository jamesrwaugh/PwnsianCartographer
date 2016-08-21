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

OBJECTS_DIR = "obj"

unix {
    LIBS += -lpng -lpthread
}

HEADERS += \
    config.h \
    types.h \
    blocks/blocks.h \
    anvil/ChunkInterface.h \
    anvil/RegionFile.h \
    anvil/RegionFileWorld.h \
    utility/keyvalues.h \
    utility/utility.h \
    utility/lodepng.h \
    utility/savepng.h \
    utility/win32_dirent.h \
    anvil/nbtutility.h \
    draw/BaseDrawer.h \
    draw/NormalDrawer.h

SOURCES += \
    config.cpp \
    main.cpp \
    blocks/blocks.cpp \
    anvil/ChunkInterface.cpp \
    anvil/RegionFile.cpp \
    anvil/RegionFileWorld.cpp \
    utility/utility.cpp \
    utility/keyvalues.cpp \
    utility/lodepng.cpp \
    utility/savepng.c \
    anvil/nbtutility.cpp \
    draw/BaseDrawer.cpp \
    draw/NormalDrawer.cpp

