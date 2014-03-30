#include "event.h"

using namespace ignis;

template<typename pT>
Event<pT>::Event(std::string type, std::string unit, bool doOutput, bool toFile):
    type(type),
    priority(IGNIS_UNSET_UINT),
    value(new double(0)),
    valueInitialized(false),
    doOutput(doOutput),
    toFile(toFile),
    unit(unit),
    nTimesExecuted(0),
    m_initialized(false),
    m_particles(MainMesh<pT>::getCurrentParticles())
{
    totalCounter++;
}


template<typename pT>
void Event<pT>::storeEvent()
{
    if (!toFile) {
        return;
    }

    observables(*loopCycle, id) = *value;

}

template<typename pT>
void Event<pT>::setOutputVariables()
{
    if (toFile) {
        id = toFileCounter++;
        outputTypes.push_back(type + ("@" + meshField->description));
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
      << "<" << setw(20) << type << " "
      << "@" << setw(30) << meshField->description;

    if (valueInitialized){
        tail << "value: " << setprecision(3) << getMeasurement() << " " << unit;
        valueInitialized = false;
    }

    tail << " >";

    s << right << tail.str();


    return s.str();
}

/*
   Static member variables:
*/

template<typename pT>
const uint * Event<pT>::loopCycle;

template<typename pT>
std::vector<std::string> Event<pT>::outputTypes;

template<typename pT>
mat Event<pT>::observables;

template<typename pT>
uint Event<pT>::nCycles = 0;

template<typename pT>
uint Event<pT>::totalCounter = 0;

template<typename pT>
uint Event<pT>::toFileCounter = 0;

template<typename pT>
uint Event<pT>::priorityCounter = 0;
