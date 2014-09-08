CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXX = gcc

!noccache {
    QMAKE_CXX = ccache $$QMAKE_CXX
}

COMMON_CXXFLAGS = -std=c++11

### MPI Settings
mpi {
    DEFINES += USE_MPI
    QMAKE_CXX = mpicxx

    QMAKE_LINK = $$QMAKE_CXX

    QMAKE_LFLAGS += $$system(mpicxx --showme:link)
    COMMON_CXXFLAGS += $$system(mpicxx --showme:compile) -DMPICH_IGNORE_CXX_SEEK
}

QMAKE_CXXFLAGS += \
    $$COMMON_CXXFLAGS

QMAKE_CXXFLAGS_DEBUG += \
    $$COMMON_CXXFLAGS

QMAKE_CXXFLAGS_RELEASE += \
    $$COMMON_CXXFLAGS \
    -O3 \
    -DNDEBUG \
    -DARMA_NO_DEBUG

QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE += -O3

QMAKE_LFLAGS_DEBUG += -g




DEFINES += \
    ARMA_MAT_PREALLOC=3

UTILS = $$PWD/utils

INCLUDEPATH += $$UTILS

LIBS += -larmadillo -llapack -lblas -L$$UTILS/DCViz/lib -lDCViz


TOP_PWD = $$PWD


