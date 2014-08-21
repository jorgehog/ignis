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

    Event(std::string type = "Event", std::string unit = "", bool doOutput=false, bool toFile=false);

    virtual ~Event();

    static void resetEventParameters();

    void _executeEvent()
    {
        execute();
        m_nTimesExecuted++;
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
        return id;
    }

    uint getAddress() const
    {
        return address;
    }


    void setPriority();

    void setManualPriority(uint p = IGNIS_UNSET_UINT);

    const uint & nTimesExecuted() const
    {
        return m_nTimesExecuted;
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
        return type;
    }

    bool shouldToFile() const
    {
        return toFile;
    }

    bool notSilent() const
    {
        return doOutput;
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
        return *value;
    }


    void setValue(double value){
        valueInitialized = true;
        *(this->value) = value;
    }

    void setValue(){
        valueInitialized = true;
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

    static void saveEventMatrix(std::string filepath)
    {
        observables(span(0, *loopCycle/MainMesh<pT>::saveValuesSpacing()), span::all).eval().save(filepath);
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


    std::string type;


    static std::vector<std::string> outputTypes;


    static uint priorityCounter;

    uint priority;

    uint address; //! This event's index in meshfield's event array


    static uint toFileCounter;

    static uint totalCounter;

    uint id;

    double* value;

    bool valueInitialized;

    bool doOutput;

    bool toFile;

    std::string unit;

    static mat observables;

    MeshField<pT> *meshField;



    virtual void execute() = 0;

    uint m_nTimesExecuted;

    bool m_initialized;


    uint onsetTime = IGNIS_UNSET_UINT;

    uint offsetTime = IGNIS_UNSET_UINT;


    const pT &particles(const uint n, const uint d) const
    {
        return (*m_particles)(n, d);
    }

    PositionHandler<pT> & particles() const
    {
        return *m_particles;
    }

    uint totalNumberOfParticles() const
    {
        return m_particles->count();
    }

private:

    PositionHandler<pT> *m_particles;

};

#include "event.cpp"

}

