#include "event.h"

using namespace ignis;

template<typename pT>
Event<pT>::Event(std::string type, std::string unit, bool doOutput, bool toFile):
    m_eventLength(IGNIS_UNSET_UINT),
    m_nCycles(IGNIS_UNSET_UINT),
    m_priority(IGNIS_UNSET_UINT),
    m_type(type),
    m_value(new double(0)),
    m_valueSetThisCycle(false),
    m_hasOutput(doOutput),
    m_storeValue(toFile),
    m_unit(unit),
    m_cycle(0),
    m_initialized(false),
    m_registeredHandler(MainMesh<pT>::currentParticles())
{
    m_refCounter++;
}

template<typename pT>
Event<pT>::~Event()
{
    m_refCounter--;

    if (m_refCounter == 0)
    {
        m_priorityCounter = 0;
    }

    delete m_value;
}


template<typename pT>
void Event<pT>::_setPriority()
{
    if (m_priority == IGNIS_UNSET_UINT)
    {
        m_priority = m_priorityCounter++;
    }
}

template<typename pT>
void Event<pT>::setManualPriority(uint p)
{
    if (p == IGNIS_UNSET_UINT)
    {
        m_priority = m_refCounter - 1;
    }

    else
    {
        m_priority = p;
        //SHIFT GREATER EQUAL + 1
    }
}

template<typename pT>
void Event<pT>::_setExplicitTimes()
{

    BADAss(m_nCycles, !=, IGNIS_UNSET_UINT, "Unset number of cycles.");

    if (m_onsetTime == IGNIS_UNSET_UINT)
    {
        setOnsetTime(0);
    }

    if (m_offsetTime == IGNIS_UNSET_UINT)
    {
        setOffsetTime(m_nCycles-1);
    }

    m_eventLength = m_offsetTime - m_onsetTime;

    BADAss(m_offsetTime, >=, m_onsetTime);
    BADAss(m_offsetTime, <, m_nCycles);
    BADAss(m_onsetTime, <, m_nCycles);

}


template<typename pT>
std::string Event<pT>::dumpString()
{
    using namespace std;

    stringstream s, tail;

    s << left
      << "<" << setw(20) << m_type << " "
      << "@" << setw(30) << m_meshField->description();

    if (m_valueSetThisCycle){
        tail << "value: " << setprecision(3) << value() << " " << m_unit;
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
uint Event<pT>::m_refCounter = 0;

template<typename pT>
uint Event<pT>::m_priorityCounter = 0;
