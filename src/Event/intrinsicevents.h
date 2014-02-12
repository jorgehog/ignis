#ifndef INTRINSICEVENTS_H
#define INTRINSICEVENTS_H

#include "event.h"
#include "../Ensemble/ensemble.h"

namespace ignis
{

class _dumpEvents : public Event {
public:


    _dumpEvents(MainMesh* mainMesh) : Event("INTRINSIC_EVENT_DUMP"), mainMesh(mainMesh) {}

    void execute() {
        mainMesh->dumpEvents();
    }

private:

    MainMesh* mainMesh;

};

class _dumpEventsToFile : public Event {
public:

    _dumpEventsToFile(MainMesh* mainMesh) : Event("INTRINSIC_EVENT_FILEDUMP"), mainMesh(mainMesh) {}

    void initialize() {
        Event::initializeEventMatrix();
    }

    void execute() {
        mainMesh->dumpEventsToFile();
    }


private:

    MainMesh* mainMesh;

};


}

#endif // INTRINSICEVENTS_H
