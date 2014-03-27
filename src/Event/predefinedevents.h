#pragma once

#include "../MeshField/meshfield.h"
#include "event.h"

#ifndef NO_DCVIZ
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

class periodicScaling : public Event {
public:

    periodicScaling() : Event("PeriodicRescale") {}


    //Hey, what I mean to say is that I rescale all positions to fit the mesh _if_ they are set to
    //periodic. If not, you have to add an event to take care of collisions with walls.
    void execute()
    {

        using namespace std;
#if defined (IGNIS_PERIODIC_X) || defined (IGNIS_PERIODIC_Y) || defined (IGNIS_PERIODIC_Z)
        for (uint i = 0; i < IGNIS_N; ++i) {
#ifdef IGNIS_PERIODIC_X
            if (particles->pos(0, i) < meshField->topology(0, 0)) {
                particles->pos(0, i) += meshField->shape(0);
            }
            particles->pos(0, i) = meshField->topology(0, 0) +
                    fmod(particles->pos(0, i) - meshField->topology(0, 0), meshField->shape(0));
#endif
#ifdef IGNIS_PERIODIC_Y
            if (particles->pos(1, i) < meshField->topology(1, 0)) {
                particles->pos(1, i) += meshField->shape(1);
            }
            particles->pos(1, i) = meshField->topology(1, 0) +
                    fmod(particles->pos(1, i) - meshField->topology(1, 0), meshField->shape(1));
#endif
#ifdef IGNIS_PERIODIC_Z
            if (particles->pos(2, i) < meshField->topology(2, 0)) {
                particles->pos(2, i) += meshField->shape(2);
            }
            particles->pos(2, i) = meshField->topology(2, 0) +
                    fmod(particles->pos(2, i), meshField->shape(2));

#endif
        }
#endif
    }

};



/*
 *
 *Event for shuffeling positions very randomly..
 *
 */


class randomShuffle : public Event {
public:
    randomShuffle() : Event("shuffling") {}

    void execute() {
        for (uint i = 0; i < IGNIS_N; ++i) {
            for (uint j = 0; j < IGNIS_DIM; ++j) {
                particles->pos(j, i) = meshField->topology(j, 0) + drand48()*meshField->shape(j);
            }
        }
    }

};




/*
 *
 * Event for counting atoms
 *
 */


class countAtoms : public Event{
public:

    countAtoms() : Event("Counting atoms", "", true) {}

    void execute(){
        setValue((meshField->getPopulation()/double(IGNIS_N))/(meshField->volume));
    }

};

class ReportProgress : public Event {
public:

    ReportProgress() : Event("Progress", "%", true) {}

    void execute() {
        setValue(*loopCycle*100.0/N);
    }
};


class VolumeChange : public Event {
public:

    VolumeChange(double ratio, bool recursive) :
        Event("VolumeChange"),
        ratio(ratio),
        recursive(recursive)
    {
        assert(ratio > 0 && "RATIO CANNOT BE NEGATIVE");
    }

    void initialize() {

        topology0 = meshField->topology;
        volume0 = meshField->volume;

        k = (pow(ratio, 1.0/IGNIS_DIM) - 1)/eventLength;

    }

private:

    double k;
    double ratio;

    bool recursive;

    mat topology0;
    double volume0;

    // Event interface
protected:
    void execute() {

        double vPrev = meshField->volume;
        assert(vPrev != 0 && "Can't increase volume of empty volume.(V=0)");

        double dL = k*(nTimesExecuted + 1.0);
        mat newTopology = topology0*(1 + dL);

        meshField->setTopology(newTopology, recursive);

        double vNew = meshField->volume;
        assert(vNew != 0 && "Volume changed to zero");

        double scale = pow(vNew/vPrev, 1.0/IGNIS_DIM);
        for (const uint & i : meshField->getAtoms()) {
            particles->pos.col(i) *= scale;
        }
    }

};


class SaveToFile : public Event {
public:

    SaveToFile(std::string path, uint freq) : Event("SaveData"), path(path), freq(freq) {}

    void execute() {
        if ((*loopCycle % freq) == 0) {
            scaledPos = particles->pos;
            scaledPos.row(0)/=meshField->shape(0);
            scaledPos.row(1)/=meshField->shape(1);
            scaledPos.save((path + "/ignisPos") + (toStr(*loopCycle) + ".arma"));
        }
    }

private:

    std::string path;
    uint freq;

    mat::fixed<IGNIS_DIM, IGNIS_N> scaledPos;

};

#ifndef NO_DCVIZ
class LauchDCViz : public Event {

public:

    LauchDCViz(std::string path, double delay) : Event(), delay(delay), viz(path + "/ignisPos0.arma") {}

    void initialize() {
        viz.launch(true, delay, 16, 14);
    }

    void execute() {}

private:

    double delay;

    DCViz viz;

};
#endif

class killMe : public Event {
public:

    killMe(uint when) : Event() {setTrigger(when);}

    void execute() {
        exit(1);
    }

};


class stall : public Event {
public:

    stall(double dt) : Event("Stall"), dt(dt) {}

    void execute() {
        usleep(dt*1E6);
    }

private:
    double dt;
};


class density : public Event {
public:

    density() : Event("Density", "", true, true) {}

    void execute() {
        setValue(meshField->getPopulation()/meshField->volume);
    }

};


}
