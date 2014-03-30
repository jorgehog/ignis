include(../../ignisdefines.pri)

CONFIG -= app_bundle
CONFIG -= qt

TEMPLATE = lib 

TARGET = ../lib/ignis

LIBS += -larmadillo -lconfig++

DEFINES += ARMA_MAT_PREALLOC=3

HEADERS += \
    defines.h \
    Particles/particles.h \
    MeshField/meshfield.h \
    Event/event.h \
    MeshField/MainMesh/mainmesh.h \
    MeshField/MainMesh/intrinsicevents.h \
    Event/predefinedevents.h \
    positionhandler.h


OTHER_FILES += \
    MeshField/meshfield.cpp \
    Event/event.cpp \
    MeshField/MainMesh/mainmesh.cpp


QMAKE_CXX = gcc

COMMON_CXXFLAGS = -std=c++11


### MPI Settings
mpi {
    DEFINES += USE_MPI
    QMAKE_CXX = mpicxx

    QMAKE_LINK = $$QMAKE_CXX

    QMAKE_LFLAGS += $$system(mpicxx --showme:link)
    COMMON_CXXFLAGS += $$system(mpicxx --showme:compile) -DMPICH_IGNORE_CXX_SEEK
}


QMAKE_CXXFLAGS += $$COMMON_CXXFLAGS

QMAKE_CXXFLAGS_DEBUG += $$COMMON_CXXFLAGS

QMAKE_CXXFLAGS_RELEASE += $$COMMON_CXXFLAGS -O3 -DARMA_NO_DEBUG -DNDEBUG -g

QMAKE_LFLAGS += -g


DCViz {
    LIBS += -lpython2.7
    INCLUDEPATH += /home/jorgehog/code/DCViz/include
    DEFINES += USE_DCVIZ
}

ccache {
    QMAKE_CXX = ccache $$QMAKE_CXX
}


!equals(PWD, $${OUT_PWD}) {
    QMAKE_POST_LINK += $(COPY_DIR) $$OUT_PWD/../lib $$PWD/../
}
