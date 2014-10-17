#pragma once


#include "../defines.h"

#include "../MeshField/meshfield.h"

#include <iostream>
#include <iomanip>

#include <map>

#include "BADAss/badass.h"

using std::string;
using std::map;

namespace ignis
{

template<typename pT>
class PositionHandler;

template<typename pT>
class Event
{
public:

    Event(string type = "Event", string m_unit = "", bool m_hasOutput=false, bool m_storeValue=false);

    virtual ~Event();

    const bool &initialized() const
    {
        return m_initialized;
    }

    void markAsInitialized(const bool state = true)
    {
        m_initialized = state;
    }

    virtual void execute() = 0;

    virtual void initialize(){}

    virtual void reset(){}

    uint meshAddress() const
    {
        return m_meshAddress;
    }

    const uint & nCycles()
    {
        return m_nCycles;
    }

    const MeshField<pT> &meshField() const
    {
        return *m_meshField;
    }


    void _setPriority();

    void setManualPriority(uint p = IGNIS_UNSET_UINT);

    const uint &cycle() const
    {
        return m_cycle;
    }

    const uint &priority() const
    {
        return m_priority;
    }

    static const uint &priorityCounter()
    {
        return m_priorityCounter;
    }

    static const uint &refCounter()
    {
        return m_refCounter;
    }

    const string type() const
    {
        return m_type;
    }

    void setDependency(const Event<pT> *event)
    {
        m_dependancies[event->type()] = event;
    }

    void setDependency(const Event<pT> &event)
    {
        setDependency(&event);
    }

    const map<const string, const Event<pT> *> &dependencies() const
    {
        return m_dependancies;
    }

    template<typename T>
    const T *dependency(const string dependencyType) const
    {
        const auto &loot = m_dependancies.find(dependencyType);

        BADAss(loot, !=, m_dependancies.end(), "Dependency not found.", [&] ()
        {
            BADAssSimpleDump(m_type, dependencyType);
        });

        return static_cast<const T*>(loot->second);
    }

    const Event<pT> *dependency(const string dependencyType) const
    {
        return dependency<const Event<pT> >(dependencyType);
    }

    const bool &storeValue() const
    {
        return m_storeValue;
    }

    bool hasOutput() const
    {
        return m_hasOutput;
    }

    string unit() const
    {
        return m_unit;
    }

    const uint &onsetTime() const
    {
        return m_onsetTime;
    }

    bool hasStarted() const
    {
        if (onsetTime() == IGNIS_UNSET_UINT)
        {
            return false;
        }

        return loopCycle() >= m_onsetTime;
    }

    const uint &offsetTime() const
    {
        return m_offsetTime;
    }

    bool hasEnded() const
    {
        return m_cycle > m_eventLength;
    }

    bool isActive() const
    {
        return !hasEnded();
    }

    const uint &eventLength() const
    {
        return m_eventLength;
    }

    double value() const
    {
        return *m_value;
    }

    uint loopCycle() const
    {
        return *m_loopCycle;
    }

    void setValue(double value)
    {
        m_valueSetThisCycle = true;
        *(this->m_value) = value;
    }

    void setValue()
    {
        m_valueSetThisCycle = true;
    }

    void setMeshField(MeshField<pT> *meshField)
    {
        this->m_meshField = meshField;
    }

    void _setNumberOfCycles(const uint nCycles)
    {
        m_nCycles = nCycles;
    }

    void _setLoopCyclePtr(const uint* loopCycle)
    {
        m_loopCycle = loopCycle;
    }

    void setAddress(uint i)
    {
        m_meshAddress = i;
    }

    void _setExplicitTimes();

    string dumpString();

    string description() const;

    void setOnsetTime(uint onsetTime)
    {

        if (onsetTime == IGNIS_UNSET_UINT) return;

        this->m_onsetTime = onsetTime;

    }

    void setOffsetTime(uint offTime)
    {

        if (offTime == IGNIS_UNSET_UINT) return;
        m_offsetTime = offTime;

    }

    void setTrigger(uint t)
    {

        setOnsetTime(t);
        setOffsetTime(t);

    }

    bool _hasExecuteImpl() const
    {
        //        return (&this->execute != &Event::execute);
        return true;
    }

    bool _hasResetImpl() const
    {
        //        return (&this->reset != &Event::reset);
        return true;
    }

    void _iterateCycle()
    {
        m_cycle++;
    }



protected:

    uint m_eventLength;

    uint m_nCycles;

    const uint *m_loopCycle;

    static uint m_refCounter;


    static uint m_priorityCounter;


    uint m_priority;

    uint m_meshAddress; //! This event's index in meshfield's event array


    const string m_type;

    double* m_value;

    bool m_valueSetThisCycle;

    const bool m_hasOutput;

    const bool m_storeValue;

    const string m_unit;


    MeshField<pT> *m_meshField;

    uint m_cycle;

    bool m_initialized;


    uint m_onsetTime;

    uint m_offsetTime;


    const pT registeredHandler(const uint n, const uint d) const
    {
        return (*m_registeredHandler)(n, d);
    }

    pT &registeredHandler(const uint n, const uint d)
    {
        return (*m_registeredHandler)(n, d);
    }

    PositionHandler<pT> & registeredHandler() const
    {
        return *m_registeredHandler;
    }

    uint totalNumberOfParticles() const
    {
        return m_registeredHandler->count();
    }

    void terminateLoop(std::string terminateMessage = "")
    {
        m_meshField->terminateLoop(terminateMessage, description());
    }

private:

    PositionHandler<pT> *m_registeredHandler;

    map<const string, const Event<pT> *> m_dependancies;

};

typedef Event<double> MeshEvent;
typedef Event<float>  fMeshEvent;
typedef Event<int>    iLatticeEvent;
typedef Event<uint>   LatticeEvent;

}

#include "event.cpp"

