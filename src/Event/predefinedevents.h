#pragma once

#include "event.h"

#include "../MeshField/meshfield.h"

#include "../positionhandler.h"


#ifdef USE_DCVIZ
#include <DCViz.h>
#endif

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

    using Event<pT>::particles;
    using Event<pT>::meshField;

    periodicScaling() : Event<pT>("PeriodicRescale") {}


    //Hey, what I mean to say is that I rescale all positions to fit the mesh _if_ they are set to
    //periodic. If not, you have to add an event to take care of collisions with walls.
    void execute()
    {

        using namespace std;

        for (uint i = 0; i < particles().count(); ++i) {
            if (particles()(i, 0) < meshField->topology(0, 0)) {
                particles()(i, 0) += meshField->shape(0);
            }
            particles()(i, 0) = meshField->topology(0, 0) +
                    fmod(particles()(i, 0) - meshField->topology(0, 0), meshField->shape(0));

            if (particles()(i, 1) < meshField->topology(1, 0)) {
                particles()(i, 1) += meshField->shape(1);
            }
            particles()(i, 1) = meshField->topology(1, 0) +
                    fmod(particles()(i, 1) - meshField->topology(1, 0), meshField->shape(1));

#if IGNIS_DIM == 3
            if (particles()(i, 2) < meshField->topology(2, 0)) {
                particles()(i, 2) += meshField->shape(2);
            }
            particles()(i, 2) = meshField->topology(2, 0) +
                    fmod(particles()(i, 2), meshField->shape(2));

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

        for (uint i = 0; i < Event<pT>::particles().count(); ++i) {
            for (uint j = 0; j < IGNIS_DIM; ++j) {
                Event<pT>::particles()(j, i) = Event<pT>::meshField->topology(j, 0) + (pT)(drand48()*Event<pT>::meshField->shape(j));
            }
        }
    }

};




/*
 *
 * Event for counting atoms
 *
 */


class countAtoms : public Event<>
{
public:

    countAtoms() : Event<>("Counting atoms", "", true) {}

    void execute(){
        setValue((Event<>::meshField->getPopulation()/double(Event<>::particles().count()))/(Event<>::meshField->volume));
    }

};

class ReportProgress : public Event<>
{
public:

    ReportProgress() : Event<>("Progress", "%", true) {}

    void execute() {
        setValue(*loopCycle*100.0/m_nCycles);
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

        topology0 = Event<pT>::meshField->topology;
        volume0   = Event<pT>::meshField->volume;

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

        double vPrev = Event<pT>::meshField->volume;
        assert(vPrev != 0 && "Can't increase volume of empty volume.(V=0)");

        double dL = k*(Event<pT>::nTimesExecuted + 1.0);
        Mat<pT> newTopology = topology0*(1 + dL);

        Event<pT>::meshField->setTopology(newTopology, recursive);

        pT vNew = Event<pT>::meshField->volume;
        assert(vNew != 0 && "Volume changed to zero");

        pT scale = (pT)pow(vNew/(double)vPrev, 1.0/IGNIS_DIM);
        for (const uint & i : Event<pT>::meshField->getAtoms()) {
            Event<pT>::particles().vec(i) *= scale;
        }
    }

};


template<typename pT>
class SaveToFile : public Event<pT> {
public:

    SaveToFile(std::string path, uint freq) : Event<pT>("SaveData"), path(path), freq(freq) {}

    void execute() {
        if ((*Event<pT>::loopCycle % freq) == 0) {

            scaledPos = Event<pT>::particles();
            scaledPos.col(0)/=Event<pT>::meshField->shape(0);
            scaledPos.col(1)/=Event<pT>::meshField->shape(1);

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

#ifdef USE_DCVIZ
class LauchDCViz : public Event<>
{

public:

    LauchDCViz(std::string path, double delay) : Event("DCViz"), delay(delay), viz(path + "/ignisPos0.arma") {}

    void initialize()
    {
        viz.launch(true, delay, 16, 14);
    }

    void execute() {}

private:

    double delay;

    DCViz viz;

};
#endif

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
        setValue(meshField->getPopulation()/(double)meshField->volume);
    }

};


}
