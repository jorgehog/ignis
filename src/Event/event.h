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


    void executeEvent() {
        execute();
        nTimesExecuted++;
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


    uint getId() const {
        return id;
    }


    void setPriority();

    void setManualPriority(uint p = IGNIS_UNSET_UINT);

    uint getPriority () const {
        return priority;
    }

    static const uint & getPriorityCounter() {
        return priorityCounter;
    }

    static const uint & getTotalCounter() {
        return totalCounter;
    }

    std::string getType() const {
        return type;
    }

    bool shouldToFile() const {
        return toFile;
    }

    bool notSilent() const {
        return doOutput;
    }

    std::string getUnit() const {
        return unit;
    }

    static uint getCounter() {
        return toFileCounter;
    }


    uint getOnsetTime() const {
        return onsetTime;
    }

    uint getOffsetTime() const {
        return offsetTime;
    }


    virtual double getMeasurement() const {
        return *value;
    }


    void setValue(double value){
        valueInitialized = true;
        *(this->value) = value;
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

        assert(offsetTime < m_nCycles);
        assert(onsetTime < m_nCycles);

    }



    void storeEvent();

    void setOutputVariables();

    static void initializeEventMatrix()
    {
        assert(m_nCycles!=0 && "Unset or empty number of cycles");

        observables.zeros(m_nCycles, getCounter());
    }

    static void dumpEventMatrixData(uint k)
    {
        for (uint i = 0; i < getCounter(); ++i)
        {
            cout << std::setw(30) << std::left << outputTypes.at(i) << "  " << std::setw(10) << observables(k, i) << endl;
        }
    }

    static void saveEventMatrix(std::string path)
    {
        observables.save(path);
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

        assert((onsetTime != IGNIS_UNSET_UINT) && ("onTime must be set before offTime."));
        assert(offTime > onsetTime && "Event must initialize before the shutdown.");
        assert(offTime != 0 && "Event offtime must be greater than 0 to execute at all.");

        offsetTime = offTime;

        eventLength = offTime - onsetTime;

    }

    void setTrigger(uint t)
    {

        setOnsetTime(t);
        setOffsetTime(t);

    }

    bool _hasExecuteImpl()
    {
        //        return (&this->execute != &Event::execute);
        return true;
    }

    bool _hasResetImpl()
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

    uint nTimesExecuted;

    bool m_initialized;


    uint onsetTime = IGNIS_UNSET_UINT;

    uint offsetTime = IGNIS_UNSET_UINT;


    const pT &particles(const uint n, const uint d)
    {
        return (*m_particles)(n, d);
    }

    PositionHandler<pT> & particles()
    {
        return *m_particles;
    }

    uint totalNumberOfParticles()
    {
        return m_particles->count();
    }

private:

    PositionHandler<pT> *m_particles;

};

#include "event.cpp"

}
