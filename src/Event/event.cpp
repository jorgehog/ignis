#include "event.h"

#include <BADAss/badass.h>

using namespace ignis;

template<typename pT>
Event<pT>::Event(std::string type, std::string unit, bool doOutput, bool toFile):
    priority(IGNIS_UNSET_UINT),
    m_type(type),
    m_value(new double(0)),
    m_valueSetThisCycle(false),
    m_hasOutput(doOutput),
    m_storeValue(toFile),
    unit(unit),
    m_cycle(0),
    m_initialized(false),
    m_registeredHandler(MainMesh<pT>::currentParticles())
{
    totalCounter++;
}

template<typename pT>
Event<pT>::~Event()
{
    totalCounter--;
    delete m_value;
}

template<typename pT>
void Event<pT>::resetEventParameters()
{
    if (totalCounter != 0)
    {
        throw std::runtime_error("Cannot reset events: Some events are still active.");
    }


    outputTypes.clear();

    observables.clear();

    toFileCounter = 0;
    priorityCounter = 0;

}


template<typename pT>
void Event<pT>::storeEvent()
{
    if (!m_storeValue)
    {
        return;
    }

    observables(*loopCycle/MainMesh<pT>::saveValuesSpacing(), m_storageID) = *m_value;



}

template<typename pT>
void Event<pT>::setOutputVariables()
{
    if (m_storeValue)
    {
        m_storageID = toFileCounter++;
        outputTypes.push_back(m_type + ("@" + meshField->m_description));
    }
}

template<typename pT>
void Event<pT>::setPriority()
{

    if (priority == IGNIS_UNSET_UINT)
    {

        priority = priorityCounter++;

    }

}

template<typename pT>
void Event<pT>::setManualPriority(uint p)
{
    if (p == IGNIS_UNSET_UINT)
    {
        priority = totalCounter;
    }

    else
    {
        priority = p;
        //SHIFT GREATER EQUAL + 1
    }
}


template<typename pT>
std::string Event<pT>::dumpString()
{
    using namespace std;

    stringstream s, tail;

    s << left
      << "<" << setw(20) << m_type << " "
      << "@" << setw(30) << meshField->m_description;

    if (m_valueSetThisCycle){
        tail << "value: " << setprecision(3) << getMeasurement() << " " << unit;
        m_valueSetThisCycle = false;
    }

    tail << " >";

    s << right << tail.str();


    return s.str();
}

/*
   Static member variables:
*/



template<typename pT>
const uint *Event<pT>::loopCycle;

template<typename pT>
std::vector<std::string> Event<pT>::outputTypes;

template<typename pT>
mat Event<pT>::observables;

template<typename pT>
uint Event<pT>::m_nCycles = 0;

template<typename pT>
uint Event<pT>::totalCounter = 0;

template<typename pT>
uint Event<pT>::toFileCounter = 0;

template<typename pT>
uint Event<pT>::priorityCounter = 0;
