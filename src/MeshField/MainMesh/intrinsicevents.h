#pragma once

#include "../../Event/event.h"

#include "mainmesh.h"

namespace ignis
{

template<typename pT>
class _reportProgress : public Event<pT>
{
public:

    using Event<pT>::loopCycle;
    using Event<pT>::m_nCycles;

    _reportProgress() : Event<pT>("Progress", "%", true) {}

    void execute() {
        this->setValue(*loopCycle*100.0/m_nCycles);
    }
};

template<typename pT>
class _dumpEvents : public Event<pT>
{
public:

    _dumpEvents(MainMesh<pT> *mm) : Event<pT>("INTRINSIC_EVENT_DUMP"), mm(mm) {}

    void execute()
    {
        if (this->m_nTimesExecuted%MainMesh<pT>::outputSpacing() == 0)
        {
            mm->dumpEvents();
        }
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
        if ((*this->loopCycle)%MainMesh<pT>::saveValuesSpacing() == 0)
        {
            mm->storeEventValues();
        }

        if ((*this->loopCycle)%MainMesh<pT>::saveFileSpacing() == 0)
        {
            Event<pT>::saveEventMatrix(mm->outputPath() + mm->filename());
        }
    }

private:

    MainMesh<pT> *mm;

};


}
