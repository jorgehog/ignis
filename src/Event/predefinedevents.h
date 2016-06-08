#pragma once

#include "event.h"

#include "../MeshField/meshfield.h"

#include "../positionhandler.h"

#include <fstream>
#include <armadillo>

#include <functional>
#include <assert.h>

using std::function;

namespace ignis
{

template<typename pT>
class BasicExecuteEvent : public Event<pT>
{
public:
    BasicExecuteEvent(std::string name, function<void(BasicExecuteEvent<pT>* event)> executeFunction) :
        Event<pT>(name),
        m_executeFunction(executeFunction)
    {

    }

    void execute()
    {
        m_executeFunction(this);
    }

private:
    const function<void(BasicExecuteEvent<pT>* event)> m_executeFunction;

};

template<typename pT>
class BasicInitializeEvent : public Event<pT>
{
public:
    BasicInitializeEvent(std::string name, function<void(BasicInitializeEvent<pT>* event)> initFunction) :
        Event<pT>(name),
        m_initFunction(initFunction)
    {

    }

    void initialize()
    {
        m_initFunction(this);
    }

    void execute()
    {
        //pass
    }

private:
    const function<void(BasicInitializeEvent<pT>* event)> m_initFunction;

};


inline void loadArmaFromIgn(arma::mat &matrix, const string path)
{
    using namespace std;
    using namespace arma;

    ifstream inFile;
    inFile.open(path, ios::binary);

    BADAssBool(inFile.good(), "Issues with opening file.", [&] ()
    {
        BADAssSimpleDump(path);
    });

    uint nRows, nCols;
    inFile.read(reinterpret_cast<char*>(&nRows), sizeof(uint));
    inFile.read(reinterpret_cast<char*>(&nCols), sizeof(uint));

    matrix.set_size(nRows, nCols);

    for (uint l = 0; l < nRows; ++l)
    {
        for (uint m = 0; m < nCols; ++m)
        {
            inFile.read(reinterpret_cast<char*>(&matrix(l, m)), sizeof(double));
        }
    }

    inFile.close();
}


/*
 *
 * Event for handling periodic boundary conditions
 *
 */

template<typename pT>
class periodicScaling : public Event<pT> {
public:

    using Event<pT>::registeredHandler;
    using Event<pT>::m_meshField;

    periodicScaling() : Event<pT>("PeriodicRescale") {}


    //Hey, what I mean to say is that I rescale all positions to fit the mesh _if_ they are set to
    //periodic. If not, you have to add an event to take care of collisions with walls.
    void execute()
    {

        using namespace std;

        for (uint i = 0; i < registeredHandler().count(); ++i) {
            if (registeredHandler()(i, 0) < m_meshField->topology(0, 0)) {
                registeredHandler()(i, 0) += m_meshField->shape(0);
            }
            registeredHandler()(i, 0) = m_meshField->topology(0, 0) +
                    fmod(registeredHandler()(i, 0) - m_meshField->topology(0, 0), m_meshField->shape(0));

            if (registeredHandler()(i, 1) < m_meshField->topology(1, 0)) {
                registeredHandler()(i, 1) += m_meshField->shape(1);
            }
            registeredHandler()(i, 1) = m_meshField->topology(1, 0) +
                    fmod(registeredHandler()(i, 1) - m_meshField->topology(1, 0), m_meshField->shape(1));

#if IGNIS_DIM == 3
            if (registeredHandler()(i, 2) < m_meshField->topology(2, 0)) {
                registeredHandler()(i, 2) += m_meshField->shape(2);
            }
            registeredHandler()(i, 2) = m_meshField->topology(2, 0) +
                    fmod(registeredHandler()(i, 2), m_meshField->shape(2));

#endif
        }
    }

};



/*
 *
 *Event for shuffeling positions very randomly..
 *
 */


template<typename pT>
class randomShuffle : public Event<pT> {
public:
    randomShuffle() : Event<pT>("shuffling") {}

    void execute() {

        for (uint i = 0; i < Event<pT>::registeredHandler().count(); ++i) {
            for (uint j = 0; j < IGNIS_DIM; ++j) {
                Event<pT>::registeredHandler()(j, i) = Event<pT>::m_meshField->topology(j, 0) + (pT)(drand48()*Event<pT>::m_meshField->shape(j));
            }
        }
    }

};




/*
 *
 * Event for counting atoms
 *
 */


template<typename pT>
class countAtoms : public Event<pT>
{
public:

    countAtoms() : Event<pT>("Counting atoms", "", true) {}

    void execute()
    {
        this->setValue(Event<pT>::m_meshField->getPopulation()/double(Event<pT>::registeredHandler().count()));
    }

};


template<typename pT>
class VolumeChange : public Event <pT>
{
public:

    VolumeChange(double ratio, bool recursive) :
        Event<pT>("VolumeChange"),
        ratio(ratio),
        recursive(recursive)
    {
        assert(ratio > 0 && "RATIO CANOT BE NEGATIVE");
    }

    void initialize() {

        topology0 = Event<pT>::m_meshField->topology;
        volume0   = Event<pT>::m_meshField->volume;

        k = (pow(ratio, 1.0/IGNIS_DIM) - 1)/Event<pT>::m_eventLength;

    }

private:

    double k;
    double ratio;

    bool recursive;

    Mat<pT> topology0;
    pT volume0;

    // Event interface
protected:
    void execute() {

        double vPrev = Event<pT>::m_meshField->volume;
        assert(vPrev != 0 && "Can't increase volume of empty volume.(V=0)");

        double dL = k*(Event<pT>::m_cycle + 1.0);
        Mat<pT> newTopology = topology0*(1 + dL);

        Event<pT>::m_meshField->setTopology(newTopology, recursive);

        pT vNew = Event<pT>::m_meshField->volume;
        assert(vNew != 0 && "Volume changed to zero");

        pT scale = (pT)pow(vNew/(double)vPrev, 1.0/IGNIS_DIM);
        for (const uint & i : Event<pT>::m_meshField->getAtoms()) {
            Event<pT>::registeredHandler().vec(i) *= scale;
        }
    }

};


template<typename pT>
class SaveToFile : public Event<pT> {
public:

    SaveToFile(std::string path, uint freq) : Event<pT>("SaveData"), path(path), freq(freq) {}

    void execute()
    {
        if ((Event<pT>::loopCycle() % freq) == 0)
        {

            scaledPos = Event<pT>::registeredHandler();
            scaledPos.col(0)/=Event<pT>::m_meshField->shape(0);
            scaledPos.col(1)/=Event<pT>::m_meshField->shape(1);

            std::stringstream s;
            s << path << "/ignisPos" << Event<pT>::loopCycle() << ".arma";
            scaledPos.save(s.str());
        }
    }

private:

    std::string path;
    uint freq;

    Mat<pT> scaledPos;

};


class killMe : public Event<> {
public:

    killMe(uint when) : Event<>()
    {
        setTrigger(when);
    }

    void execute() {
        exit(1);
    }

};


class stall : public Event<> {
public:

    stall(double dt) : Event<>("Stall"), dt(dt) {}

    void execute() {
        usleep(dt*1E6);
    }

private:
    double dt;
};


class density : public Event<> {
public:

    density() : Event<>("Density", "", true, true) {}

    void execute() {
        setValue(m_meshField->getPopulation()/(double)m_meshField->volume);
    }

};

}
