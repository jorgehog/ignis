#ifndef EVENT_H
#define EVENT_H


#include "../defines.h"

#include "../MeshField/meshfield.h"

#include <iostream>
#include <iomanip>

#include <assert.h>


namespace ignis
{

struct Ensemble;

class Event
{
protected:

    static uint N;

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


    MeshField* meshField;

    Ensemble* ensemble;


    virtual void execute() = 0;

    uint nTimesExecuted;

    bool initialized;


    uint onsetTime = IGNIS_UNSET_UINT;

    uint offsetTime = IGNIS_UNSET_UINT;

public:

    Event(std::string type = "Event", std::string unit = "", bool doOutput=false, bool toFile=false);


    void executeEvent() {
        execute();
        nTimesExecuted++;
    }

    void _initEvent() {
        if (initialized) return;

        initialized = true;
        initialize();
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

    void setEnsemble(Ensemble* ensemble){
        this->ensemble = ensemble;
    }

    void setMeshField(MeshField* meshField){
        this->meshField = meshField;
    }

    static void setNumberOfCycles(uint & N){
        Event::N = N;
    }

    static void setLoopCyclePtr(const uint* loopCycle){
        Event::loopCycle = loopCycle;
    }

    void setAddress(uint i) {
        address = i;
    }

    void setExplicitTimes() {

        if (onsetTime == IGNIS_UNSET_UINT) {
            setOnsetTime(0);
        }

        if (offsetTime == IGNIS_UNSET_UINT) {
            setOffsetTime(N-1);
        }

    }



    void storeEvent();

    void setOutputVariables();

    static void initializeEventMatrix() {
        assert(N!=0 && "Unset or empty number of cycles");

        observables.zeros(N, getCounter());
    }

    static void dumpEventMatrixData(uint k) {
        for (uint i = 0; i < getCounter(); ++i) {
            cout << std::setw(30) << std::left << outputTypes.at(i) << "  " << std::setw(10) << observables(k, i) << endl;
        }
    }

    static void saveEventMatrix(std::string path) {
        observables.save(path);
    }

    std::string dumpString();

    void setOnsetTime(uint onsetTime){

        if (onsetTime == IGNIS_UNSET_UINT) return;

        this->onsetTime = onsetTime;

    }

    void setOffsetTime(uint offTime) {

        if (offTime == IGNIS_UNSET_UINT) return;

        assert((onsetTime != IGNIS_UNSET_UINT) && ("onTime must be set before offTime."));
        assert(offTime > onsetTime && "Event must initialize before the shutdown.");
        assert(offTime != 0 && "Event offtime must be greater than 0 to execute at all.");

        offsetTime = offTime;

        eventLength = offTime - onsetTime;

    }

    void setTrigger(uint t){

        setOnsetTime(t);
        setOffsetTime(t);

    }

    bool _hasExecuteImpl() {
        //        return (&this->execute != &Event::execute);
        return true;
    }

    bool _hasResetImpl() {
        //        return (&this->reset != &Event::reset);
        return true;
    }

    uint eventLength = IGNIS_UNSET_UINT;

};

}

#endif // EVENT_H
