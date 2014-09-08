#pragma once

#include "../../Event/event.h"

#include "mainmesh.h"

namespace ignis
{

template<typename pT>
class _reportProgress : public Event<pT>
{
public:

    using Event<pT>::m_loopCycle;
    using Event<pT>::m_nCycles;

    _reportProgress() : Event<pT>("Progress", "%", true) {}

    void execute() {
        this->setValue(*m_loopCycle*100.0/m_nCycles);
    }
};

template<typename pT>
class _dumpEvents : public Event<pT>
{
public:

    _dumpEvents(MainMesh<pT> *mm) : Event<pT>("INTRINSIC_EVENT_DUMP"), mm(mm) {}

    void execute()
    {
        if (this->m_cycle%mm->outputSpacing() == 0)
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

    _dumpEventsToFile(MainMesh<pT>* mm) : Event<pT>("INTRINSIC_EVENT_FILEDUMP"), m_mm(mm) {}

    void initialize()
    {
        m_mm->_initializeEventStorage(this->m_nCycles/m_mm->saveValuesSpacing());
    }

    void execute()
    {
        if ((*this->m_loopCycle)%m_mm->saveValuesSpacing() == 0)
        {
            m_mm->_storeEventValues(*this->m_loopCycle/m_mm->saveValuesSpacing());
        }

    }

private:

    MainMesh<pT> *m_mm;

};


}
