CONFIG -= app_bundle
CONFIG -= qt
#CONFIG += 2D

TEMPLATE = lib 

TARGET = ../lib/ignis

LIBS += -larmadillo

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

QMAKE_LFLAGS_DEBUG += -g

QMAKE_CXXFLAGS_RELEASE += $$COMMON_CXXFLAGS -DARMA_NO_DEBUG -DNDEBUG

QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE += -O3

INCLUDEPATH += ../utils

QMAKE_CLEAN += ../lib/lib*

2D  {

    DEFINES += IGNIS_DIM=2

}

ccache {
    QMAKE_CXX = ccache $$QMAKE_CXX
}

QMAKE_PRE_LINK += $(MKDIR) $$PWD/../lib $$shadowed($$PWD)/../lib

!equals(PWD, $${OUT_PWD}) {
    QMAKE_POST_LINK += $(COPY_DIR) $$OUT_PWD/../lib $$PWD/../
}
