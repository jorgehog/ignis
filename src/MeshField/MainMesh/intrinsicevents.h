#pragma once

#include "../../Event/event.h"

#include "mainmesh.h"

namespace ignis
{

template<typename pT>
class _dumpEvents : public Event<pT>
{
public:

    _dumpEvents(MainMesh<pT> *mm) : Event<pT>("INTRINSIC_EVENT_DUMP"), mm(mm) {}

    void execute()
    {
        mm->dumpEvents();
    }

private:

    MainMesh<pT> *mm;

};

template<typename pT>
class _dumpEventsToFile : public Event<pT>
{
public:

    _dumpEventsToFile(MainMesh<pT>* mm) : Event<pT>("INTRINSIC_EVENT_FILEDUMP"), mm(mm) {}

    void initialize()
    {
        Event<pT>::initializeEventMatrix();
    }

    void execute()
    {
        mm->dumpEventsToFile();
    }

private:

    MainMesh<pT> *mm;

};


}
