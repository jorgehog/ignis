#pragma once


#include "../defines.h"

#include "../MeshField/meshfield.h"

#include <iostream>
#include <iomanip>

#include <assert.h>


namespace ignis
{

template<typename pT>
class PositionHandler;

template<typename pT>
class Event
{
public:

    static const uint & nCycles()
    {
        return m_nCycles;
    }

    Event(std::string type = "Event", std::string unit = "", bool m_hasOutput=false, bool m_storeValue=false);

    virtual ~Event();

    static void resetEventParameters();

    void _executeEvent()
    {
        execute();
        m_cycle++;
    }

    const bool & initialized() const
    {
        return m_initialized;
    }

    void markAsInitialized()
    {
        m_initialized = true;
    }

    virtual void initialize(){}

    virtual void reset(){}


    uint getId() const
    {
        return m_storageID;
    }

    uint getAddress() const
    {
        return address;
    }


    void setPriority();

    void setManualPriority(uint p = IGNIS_UNSET_UINT);

    const uint & cycle() const
    {
        return m_cycle;
    }

    uint getPriority () const
    {
        return priority;
    }

    static const uint & getPriorityCounter()
    {
        return priorityCounter;
    }

    static const uint & getTotalCounter()
    {
        return totalCounter;
    }

    std::string getType() const
    {
        return m_type;
    }

    bool shouldToFile() const
    {
        return m_storeValue;
    }

    bool notSilent() const
    {
        return m_hasOutput;
    }

    std::string getUnit() const
    {
        return unit;
    }

    static uint getCounter()
    {
        return toFileCounter;
    }


    uint getOnsetTime() const
    {
        return onsetTime;
    }

    uint getOffsetTime() const
    {
        return offsetTime;
    }


    double getMeasurement() const
    {
        return *m_value;
    }


    void setValue(double value){
        m_valueSetThisCycle = true;
        *(this->m_value) = value;
    }

    void setValue(){
        m_valueSetThisCycle = true;
    }

    void setMeshField(MeshField<pT> *meshField)
    {
        this->meshField = meshField;
    }

    static void setNumberOfCycles(uint & N){
        Event<pT>::m_nCycles = N;
    }

    static void setLoopCyclePtr(const uint* loopCycle){
        Event<pT>::loopCycle = loopCycle;
    }

    void setAddress(uint i) {
        address = i;
    }

    void setExplicitTimes()
    {

        if (onsetTime == IGNIS_UNSET_UINT) {
            setOnsetTime(0);
        }

        if (offsetTime == IGNIS_UNSET_UINT) {
            setOffsetTime(m_nCycles-1);
        }

        eventLength = offsetTime - onsetTime;

        assert(offsetTime >= onsetTime);
        assert(offsetTime < m_nCycles);
        assert(onsetTime < m_nCycles);

    }



    void storeEvent();

    void setOutputVariables();

    static void initializeEventMatrix()
    {
        assert(m_nCycles!=0 && "Unset or empty number of cycles");

        observables.zeros(m_nCycles/MainMesh<pT>::saveValuesSpacing(), getCounter());
    }

    static void dumpEventMatrixData(uint k)
    {
        for (uint i = 0; i < getCounter(); ++i)
        {
            cout << std::setw(30) << std::left << outputTypes.at(i) << "  " << std::setw(10) << observables(k, i) << endl;
        }
    }

    static const std::vector<std::string> &outputEventDescriptions()
    {
        return outputTypes;
    }

    static const mat &eventMatrix()
    {
        return observables;
    }

    std::string dumpString();

    void setOnsetTime(uint onsetTime)
    {

        if (onsetTime == IGNIS_UNSET_UINT) return;

        this->onsetTime = onsetTime;

    }

    void setOffsetTime(uint offTime)
    {

        if (offTime == IGNIS_UNSET_UINT) return;
        offsetTime = offTime;

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

    uint eventLength = IGNIS_UNSET_UINT;


protected:

    static uint m_nCycles;

    static const uint *loopCycle;

    static mat observables;

    static std::vector<std::string> outputTypes;

    static uint toFileCounter;

    static uint totalCounter;


    static uint priorityCounter;


    uint priority;

    uint address; //! This event's index in meshfield's event array


    const std::string m_type;

    uint m_storageID;

    double* m_value;

    bool m_valueSetThisCycle;

    const bool m_hasOutput;

    const bool m_storeValue;

    const std::string unit;


    MeshField<pT> *meshField;



    virtual void execute() = 0;

    uint m_cycle;

    bool m_initialized;


    uint onsetTime = IGNIS_UNSET_UINT;

    uint offsetTime = IGNIS_UNSET_UINT;


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

