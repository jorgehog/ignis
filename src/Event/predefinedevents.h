#pragma once

#include "event.h"

#include "../MeshField/meshfield.h"

#include "../positionhandler.h"

#include <DCViz/include/DCViz.h>


#include <boost/lexical_cast.hpp>


/*
 *
 * Event for handling periodic boundary conditions
 *
 */

namespace ignis
{

template<typename pT>
class periodicScaling : public Event<pT> {
public:

    using Event<pT>::registeredHandler;
    using Event<pT>::meshField;

    periodicScaling() : Event<pT>("PeriodicRescale") {}


    //Hey, what I mean to say is that I rescale all positions to fit the mesh _if_ they are set to
    //periodic. If not, you have to add an event to take care of collisions with walls.
    void execute()
    {

        using namespace std;

        for (uint i = 0; i < registeredHandler().count(); ++i) {
            if (registeredHandler()(i, 0) < meshField->m_topology(0, 0)) {
                registeredHandler()(i, 0) += meshField->m_shape(0);
            }
            registeredHandler()(i, 0) = meshField->m_topology(0, 0) +
                    fmod(registeredHandler()(i, 0) - meshField->m_topology(0, 0), meshField->m_shape(0));

            if (registeredHandler()(i, 1) < meshField->m_topology(1, 0)) {
                registeredHandler()(i, 1) += meshField->m_shape(1);
            }
            registeredHandler()(i, 1) = meshField->m_topology(1, 0) +
                    fmod(registeredHandler()(i, 1) - meshField->m_topology(1, 0), meshField->m_shape(1));

#if IGNIS_DIM == 3
            if (registeredHandler()(i, 2) < meshField->m_topology(2, 0)) {
                registeredHandler()(i, 2) += meshField->m_shape(2);
            }
            registeredHandler()(i, 2) = meshField->m_topology(2, 0) +
                    fmod(registeredHandler()(i, 2), meshField->m_shape(2));

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
                Event<pT>::registeredHandler()(j, i) = Event<pT>::meshField->m_topology(j, 0) + (pT)(drand48()*Event<pT>::meshField->m_shape(j));
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
        this->setValue(Event<pT>::meshField->getPopulation()/double(Event<pT>::registeredHandler().count()));
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

        topology0 = Event<pT>::meshField->m_topology;
        volume0   = Event<pT>::meshField->m_volume;

        k = (pow(ratio, 1.0/IGNIS_DIM) - 1)/Event<pT>::eventLength;

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

        double vPrev = Event<pT>::meshField->m_volume;
        assert(vPrev != 0 && "Can't increase volume of empty volume.(V=0)");

        double dL = k*(Event<pT>::m_cycle + 1.0);
        Mat<pT> newTopology = topology0*(1 + dL);

        Event<pT>::meshField->setTopology(newTopology, recursive);

        pT vNew = Event<pT>::meshField->m_volume;
        assert(vNew != 0 && "Volume changed to zero");

        pT scale = (pT)pow(vNew/(double)vPrev, 1.0/IGNIS_DIM);
        for (const uint & i : Event<pT>::meshField->getAtoms()) {
            Event<pT>::registeredHandler().vec(i) *= scale;
        }
    }

};


template<typename pT>
class SaveToFile : public Event<pT> {
public:

    SaveToFile(std::string path, uint freq) : Event<pT>("SaveData"), path(path), freq(freq) {}

    void execute() {
        if ((*Event<pT>::loopCycle % freq) == 0) {

            scaledPos = Event<pT>::registeredHandler();
            scaledPos.col(0)/=Event<pT>::meshField->m_shape(0);
            scaledPos.col(1)/=Event<pT>::meshField->m_shape(1);

            std::stringstream s;
            s << path << "/ignisPos" << *Event<pT>::loopCycle << ".arma";
            scaledPos.save(s.str());
        }
    }

private:

    std::string path;
    uint freq;

    Mat<pT> scaledPos;

};

template<typename pT>
class LauchDCViz : public Event<pT>
{

public:

    LauchDCViz(const std::string path,
               const double delay = 0.0,
               const bool dynamic = true,
               const int sx = 16,
               const int sy = 14) :
        Event<pT>("DCViz"),
        m_delay(delay),
        m_viz(path),
        m_dynamic(dynamic),
        m_sx(sx),
        m_sy(sy)
    {

    }

    void initialize()
    {
        m_viz.launch(m_dynamic, m_delay, m_sx, m_sy);
    }

    void execute() {}

private:

    const double m_delay;

    DCViz m_viz;

    const bool m_dynamic;

    const int m_sx;

    const int m_sy;

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
        setValue(meshField->getPopulation()/(double)meshField->m_volume);
    }

};


}
