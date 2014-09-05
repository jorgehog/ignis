CONFIG -= app_bundle
CONFIG -= qt
#CONFIG += 2D

TEMPLATE = lib 

TARGET = ../lib/ignis

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



2D  {

    DEFINES += IGNIS_DIM=2

}


QMAKE_CLEAN += ../lib/lib*

QMAKE_PRE_LINK += $(MKDIR) $$PWD/../lib $$shadowed($$PWD)/../lib

!equals(PWD, $${OUT_PWD}) {
    QMAKE_POST_LINK += $(COPY_DIR) $$OUT_PWD/../lib $$PWD/../
}
