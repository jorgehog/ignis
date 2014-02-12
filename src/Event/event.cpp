#include "event.h"

#include "../defines.h"

#include <sstream>
#include <iomanip>

using namespace ignis;


Event::Event(std::string type, std::string unit, bool doOutput, bool toFile):
    type(type),
    priority(IGNIS_UNSET_UINT),
    value(new double(0)),
    valueInitialized(false),
    doOutput(doOutput),
    toFile(toFile),
    unit(unit),
    nTimesExecuted(0),
    initialized(false)
{
    totalCounter++;
}

void Event::storeEvent()
{
    if (!toFile) {
        return;
    }
    observables(*loopCycle, id) = *value;

}

void Event::setOutputVariables()
{
    if (toFile) {
        id = toFileCounter++;
        outputTypes.push_back(type + ("@" + meshField->description));
    }
}

void Event::setPriority()
{

    if (priority == IGNIS_UNSET_UINT)
    {

        priority = priorityCounter++;

    }

}

void Event::setManualPriority(uint p)
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


std::string Event::dumpString()
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

const uint * Event::loopCycle;

std::vector<std::string> Event::outputTypes;

mat Event::observables;

uint Event::N = 0;

uint Event::totalCounter = 0;
uint Event::toFileCounter = 0;
uint Event::priorityCounter = 0;
