#pragma once


#include "../defines.h"

#include "../MeshField/meshfield.h"

#include <iostream>
#include <iomanip>

#include "BADAss/badass.h"


namespace ignis
{

template<typename pT>
class PositionHandler;

template<typename pT>
class Event
{
public:

    Event(std::string type = "Event", std::string m_unit = "", bool m_hasOutput=false, bool m_storeValue=false);

    virtual ~Event();

    void _executeEvent()
    {
        execute();
        m_cycle++;
    }

    const bool &initialized() const
    {
        return m_initialized;
    }

    void markAsInitialized()
    {
        m_initialized = true;
    }

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

    const std::string type() const
    {
        return m_type;
    }

    const bool &storeValue() const
    {
        return m_storeValue;
    }

    bool notSilent() const
    {
        return m_hasOutput;
    }

    std::string unit() const
    {
        return m_unit;
    }

    uint onsetTime() const
    {
        return m_onsetTime;
    }

    uint offsetTime() const
    {
        return m_offsetTime;
    }


    double value() const
    {
        return *m_value;
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

    void _setLoopCyclePtrePtr(const uint* loopCycle)
    {
        m_loopCycle = loopCycle;
    }

    void setAddress(uint i)
    {
        m_meshAddress = i;
    }

    void _setExplicitTimes();

    std::string dumpString();

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

    uint m_eventLength = IGNIS_UNSET_UINT;


protected:

    uint m_nCycles;

    const uint *m_loopCycle;

    static uint m_refCounter;


    static uint m_priorityCounter;


    uint m_priority;

    uint m_meshAddress; //! This event's index in meshfield's event array


    const std::string m_type;

    double* m_value;

    bool m_valueSetThisCycle;

    const bool m_hasOutput;

    const bool m_storeValue;

    const std::string m_unit;


    MeshField<pT> *m_meshField;



    virtual void execute() = 0;

    uint m_cycle;

    bool m_initialized;


    uint m_onsetTime = IGNIS_UNSET_UINT;

    uint m_offsetTime = IGNIS_UNSET_UINT;


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

private:

    PositionHandler<pT> *m_registeredHandler;

};

typedef Event<double> MeshEvent;
typedef Event<float>  fMeshEvent;
typedef Event<int>    iLatticeEvent;
typedef Event<uint>   LatticeEvent;

}

#include "event.cpp"

