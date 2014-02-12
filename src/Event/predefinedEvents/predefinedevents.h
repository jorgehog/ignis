#ifndef PREDEFINEDEVENTS_H
#define PREDEFINEDEVENTS_H


#include "../../defines.h"

#include "../../Event/event.h"
#include "../../Ensemble/ensemble.h"
#include "../../MeshField/meshfield.h"
#include "../../gears.h"

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
            if (ensemble->pos(0, i) < meshField->topology(0, 0)) {
                ensemble->pos(0, i) += meshField->shape(0);
            }
            ensemble->pos(0, i) = meshField->topology(0, 0) +
                    fmod(ensemble->pos(0, i) - meshField->topology(0, 0), meshField->shape(0));
#endif
#ifdef IGNIS_PERIODIC_Y
            if (ensemble->pos(1, i) < meshField->topology(1, 0)) {
                ensemble->pos(1, i) += meshField->shape(1);
            }
            ensemble->pos(1, i) = meshField->topology(1, 0) +
                    fmod(ensemble->pos(1, i) - meshField->topology(1, 0), meshField->shape(1));
#endif
#ifdef IGNIS_PERIODIC_Z
            if (ensemble->pos(2, i) < meshField->topology(2, 0)) {
                ensemble->pos(2, i) += meshField->shape(2);
            }
            ensemble->pos(2, i) = meshField->topology(2, 0) +
                    fmod(ensemble->pos(2, i), meshField->shape(2));

#endif
        }
#endif
    }

};

/*
 * Velocity verlet. Due to it's nature it has to be split to fit the event system.
 */

class VelocityVerletFirstHalf : public Event {
public:

    VelocityVerletFirstHalf(double dt) : Event("VelVer1"), dt(dt) {}

    void execute() {

        double m;
        for (uint i = 0; i < IGNIS_N; ++i) {

            m = ensemble->masses(i%ensemble->nSpecies);

            for (uint k = 0; k < IGNIS_DIM; ++k) {
                ensemble->vel(k, i) += ensemble->forces(k, i)*dt/(2*m);
                ensemble->pos(k, i) += ensemble->vel(k, i)*dt;
            }

        }

    }

private:
    double dt;

};

class VelocityVerletSecondHalf : public Event {
public:

    VelocityVerletSecondHalf(double dt) : Event("VelVer2"), dt(dt) {}

    void execute() {

        double m;
        for (uint i = 0; i < IGNIS_N; ++i) {

            m = ensemble->masses(i%ensemble->nSpecies);
            for (uint k = 0; k < IGNIS_DIM; ++k) {
                ensemble->vel(k, i) += ensemble->forces(k, i)*dt/(2*m);
            }
        }
    }

private:

    double dt;

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
        ensemble->pos.randu();
        for (uint i = 0; i < IGNIS_N; ++i) {
            for (uint j = 0; j < IGNIS_DIM; ++j) {
                ensemble->pos(j, i) = meshField->topology(j, 0) + ensemble->pos(j, i)*meshField->shape(j);
            }
        }
    }

};


/*
 *
 * Event for thermostats

 *
 */

class thermostat : public Event {
public:
    thermostat(const double & T0, const double & tau, const double & dt) :
        Event("Thermostat", "T0", true, true), T0(T0), tau(tau), dt(dt) {}

protected:

    double T0;
    double tau;
    double dt;

    void setT(){
        setValue(gears::getTemperature(meshField));
    }



};

class BerendsenThermostat : public thermostat {
public:
    BerendsenThermostat(const double & T0, const double & tau, const double & dt,
                        uint onTime = IGNIS_UNSET_UINT,
                        uint offTime = IGNIS_UNSET_UINT) :
        thermostat(T0, tau, dt) {
        setOnsetTime(onTime);
        setOffsetTime(offTime);
    }

    void execute() {
        getGamma();

        for (const uint & i : meshField->getAtoms()) {
            ensemble->vel(0, i) *= gamma;
            ensemble->vel(1, i) *= gamma;
#if IGNIS_DIM == 3
            ensemble->vel(2, i) *= gamma;
#endif
        }
    }

protected:

    double gamma;

    void getGamma(){
        setT();
        gamma = sqrt(1 + dt/tau*(T0/getMeasurement() - 1));
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

class ContractMesh : public Event {
public:

    //delta = L_new/L_old -- fraction of shrink/expand
    //trigger = at which time should we trigger?
    //xyz = direction (0=x, 1=y ...)
    ContractMesh(double delta, uint xyz,
                 uint onTime = IGNIS_UNSET_UINT,
                 uint offTime = IGNIS_UNSET_UINT) :
        Event("CompressMesh"),
        delta(delta),
        xyz(xyz)
    {

        assert(xyz < IGNIS_DIM);
        assert(delta > 0);

        setOnsetTime(onTime);
        setOffsetTime(offTime);

    }

    void initialize() {
        double L0 = meshField->shape(xyz);
        deltaL = (1 - delta)*L0/(eventLength);
    }

    void execute() {

        double L = meshField->shape(xyz); //The length
        double C = meshField->topology(xyz, 0) + L/2; //The centerpoint

        double localDelta = 1 - deltaL/(2*L);


        meshField->stretchField(deltaL, xyz);

        for (uint i = 0; i < IGNIS_N; ++i) {
            ensemble->pos(xyz, i) =  C*(1-localDelta) + localDelta*ensemble->pos(xyz, i);
        }

    }

protected:

    double delta;
    uint xyz;
    double deltaL;

};

class ExpandMesh : public ContractMesh {
public:

    ExpandMesh(double delta, uint xyz,
               bool pull = false,
               uint onTime = IGNIS_UNSET_UINT,
               uint offTime = IGNIS_UNSET_UINT) :
        ContractMesh(delta, xyz, onTime, offTime),
        pull(pull)
    {

        type = "ExpandMesh";
        assert(delta > 1 && "Expansion must have delta>1");

    }

    void execute() {

        meshField->stretchField(deltaL, xyz);

        if (!pull) return;

        double L = meshField->shape(xyz); //The length
        double C = meshField->topology(xyz, 0) + L/2; //The centerpoint
        double localDelta = 1 - deltaL/(2*L);

        for (uint i = 0; i < IGNIS_N; ++i) {
            ensemble->pos(xyz, i) =  C*(1-localDelta) + localDelta*ensemble->pos(xyz, i);
        }

    }


private:

    bool pull;

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
            ensemble->pos.col(i) *= scale;
        }
    }

};


class SaveToFile : public Event {
public:

    SaveToFile(std::string path, uint freq) : Event("SaveData"), path(path), freq(freq) {}

    void execute() {
        if ((*loopCycle % freq) == 0) {
            scaledPos = ensemble->pos;
            scaledPos.row(0)/=meshField->shape(0);
            scaledPos.row(1)/=meshField->shape(1);
            scaledPos.save((path + "/mdPos") + (toStr(*loopCycle) + ".arma"));
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

    LauchDCViz(std::string path, double delay) : Event(), delay(delay), viz(path + "/mdPos0.arma") {}

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

class debugSubMeshResize : public Event {
public:

    debugSubMeshResize(MeshField *mainMesh) : Event("debubSubMesh", "", true), mainMesh(mainMesh) {}

    void initialize() {
        R0 = meshField->volume/mainMesh->volume;
    }

    void execute(){
        double R = meshField->volume/mainMesh->volume;

        if (R != R){
            std::cout << meshField->volume << std::endl;
            std::cout << mainMesh->volume << std::endl;
            exit(1);
        }

        setValue(R/R0);
    }

private:
    MeshField * mainMesh;
    double R0;


};

class dummy : public Event {
public:
    void execute() {}
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


class pressureMOP : public Event {
public:

    pressureMOP(uint xyz) : Event("Pressure", "P0", true, true), xyz(xyz) {}

    void initialize() {
        mat topology1 = meshField->topology;
        mat topology2 = meshField->topology;

        topology1(xyz, 1) = topology1(xyz, 0) + meshField->shape(xyz)/2;
        topology2(xyz, 0) = topology1(xyz, 1);

        box1 = new MeshField(topology1, *ensemble, "Method of Planes 1. field");
        box2 = new MeshField(topology2, *ensemble, "Method of Planes 2. field");

        meshField->addSubField(*box1);
        meshField->addSubField(*box2);

        area = 1;

        for (uint i = 0; i < IGNIS_DIM; ++i) {
            if (i != xyz) {
                area *= meshField->shape(i);
            }
        }

    }

    double getPressureMOP() {

        double planeForce = 0;

        for (const uint & i : box1->getAtoms()) {
            for (const uint & j : box2->getAtoms()) {
                planeForce += ensemble->forceVectors(i, j, xyz);
            }
        }


        return fabs(planeForce/area);
    }

    void execute() {

        double pressure = getPressureMOP();

        setValue(pressure);
    }

private:
    uint xyz;
    double area;

    MeshField* box1;
    MeshField* box2;


};


class checkEnergyConservation : public Event {
public:

    checkEnergyConservation() : Event("energyConservation", "", true) {}

    void initialize() {
        EkPrev = gears::getKineticEnergy(meshField);
    }

    void execute() {
        Ek = gears::getKineticEnergy(meshField);

        setValue((Ek-EkPrev)/EkPrev);
    }

    void reset() {
        EkPrev = Ek;
    }

private:
    double Ek;
    double EkPrev;
};

class checkMomentumConservation : public Event {
public:

    checkMomentumConservation() : Event("momentumConservation", "", true) {}

    void execute() {
        vec p = gears::getTotalLinearMomentum(ensemble);

        setValue(as_scalar(sum(p)));
    }

};


class diffusionConstant : public Event {
public:

    diffusionConstant(double dt) : Event("DiffusionConstant", "D0", true), fac(dt/(IGNIS_DIM)), D(0) {}

    void initialize() {
        v0 = ensemble->vel;
    }

    void execute() {

        double dD = 0;

        for (const uint & i : meshField->getAtoms()) {
            for (uint j = 0; j < IGNIS_DIM; ++j) {
                dD += v0(j, i)*ensemble->vel(j, i);
            }
        }

        dD /= meshField->getPopulation();

        if (dD != dD) {
            return;
        }

        D += dD*fac;

        setValue(D);

    }

private:

    mat::fixed<IGNIS_DIM, IGNIS_N> v0;
    double fac;
    double D;

};

class temperatureFluctuations : public Event {
public:

    temperatureFluctuations(thermostat *t) : Event("TempFluct", "T0", true), t(t) {
        setOnsetTime(t->getOnsetTime());
        setOffsetTime(t->getOffsetTime());
    }

    void execute() {

        double T = t->getMeasurement();

        if (T != T) {
            setValue(0);
            return;
        }

        avgT += T;
        avgT2 += T*T;

        setValue(sqrt(avgT2/(nTimesExecuted+1) - avgT*avgT/((nTimesExecuted+1)*(nTimesExecuted+1))));

    }

private:

    thermostat* t;

    double avgT;
    double avgT2;

};

}

#endif // PREDEFINEDEVENTS_H
