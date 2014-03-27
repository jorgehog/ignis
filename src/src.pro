CONFIG -= app_bundle
CONFIG -= qt

TEMPLATE = lib 

TARGET = ../lib/ignis


LIBS += -lpython2.7 -larmadillo -lconfig++

DEFINES += ARMA_MAT_PREALLOC=3

INCLUDEPATH += /home/jorgehog/code/DCViz/include



SOURCES += MeshField/meshfield.cpp \
    Event/event.cpp \
    MeshField/MainMesh/mainmesh.cpp

HEADERS += \
    defines.h \
    Ensemble/ensemble.h \
    MeshField/meshfield.h \
    Event/event.h \
    MeshField/MainMesh/mainmesh.h \
    Event/predefinedEvents/predefinedevents.h \
    gears.h \
    Event/intrinsicevents.h



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


ccache {
    QMAKE_CXX = ccache $$QMAKE_CXX
}


!equals(PWD, $${OUT_PWD}) {
    QMAKE_POST_LINK += $(COPY_DIR) $$OUT_PWD/../lib $$PWD/../
}
