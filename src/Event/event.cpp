#include "event.h"

#include "../defines.h"

#include "../positionhandler.h"

#include "../MeshField/MainMesh/mainmesh.h"

#include <sstream>
#include <iomanip>

using namespace ignis;

template<typename pT, class pT2>
Event<pT, pT2>::Event(std::string type, std::string unit, bool doOutput, bool toFile):
    type(type),
    priority(IGNIS_UNSET_UINT),
    value(new double(0)),
    valueInitialized(false),
    doOutput(doOutput),
    toFile(toFile),
    unit(unit),
    m_particles(MainMesh<pT>::getCurrentParticles()),
    nTimesExecuted(0),
    m_initialized(false)
{
    totalCounter++;
}


template<typename pT, class pT2>
void Event<pT, pT2>::storeEvent()
{
    if (!toFile) {
        return;
    }

    observables(*loopCycle, id) = *value;

}

template<typename pT, class pT2>
void Event<pT, pT2>::setOutputVariables()
{
    if (toFile) {
        id = toFileCounter++;
        outputTypes.push_back(type + ("@" + meshField->description));
    }
}

template<typename pT, class pT2>
void Event<pT, pT2>::setPriority()
{

    if (priority == IGNIS_UNSET_UINT)
    {

        priority = priorityCounter++;

    }

}

template<typename pT, class pT2>
void Event<pT, pT2>::setManualPriority(uint p)
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


template<typename pT, class pT2>
std::string Event<pT, pT2>::dumpString()
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

template<typename pT, class pT2>
const uint * Event<pT, pT2>::loopCycle;

template<typename pT, class pT2>
std::vector<std::string> Event<pT, pT2>::outputTypes;

template<typename pT, class pT2>
mat Event<pT, pT2>::observables;

template<typename pT, class pT2>
uint Event<pT, pT2>::nCycles = 0;

template<typename pT, class pT2>
uint Event<pT, pT2>::totalCounter = 0;

template<typename pT, class pT2>
uint Event<pT, pT2>::toFileCounter = 0;

template<typename pT, class pT2>
uint Event<pT, pT2>::priorityCounter = 0;
