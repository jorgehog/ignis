#ifndef GEARS_H
#define GEARS_H


#include "defines.h"

#include "MeshField/meshfield.h"
#include "Ensemble/ensemble.h"

#include <armadillo>

using namespace arma;

namespace ignis
{

namespace gears {

inline vec getTotalLinearMomentum(Ensemble* ensemble) {

    vec pTot = zeros<vec>(IGNIS_DIM);

    //retrieve ensemble values;
    const vec & masses = ensemble->masses;
    mat & vel = ensemble->vel;
    const uint & N = ensemble->nSpecies;

    //Calculates total linear momentum
    for (uint k = 0; k < IGNIS_N; ++k) {
        pTot += masses(k%N)*vel.col(k);
    }

    return pTot;

}

inline void cancelLinearMomentum(Ensemble* ensemble)
{


    //retrieve ensemble values;
    const vec & masses = ensemble->masses;
    mat & vel = ensemble->vel;
    const uint & N = ensemble->nSpecies;

    vec pTot = getTotalLinearMomentum(ensemble);

    pTot /= IGNIS_N;

    //subtract the velocity in such a way that the momentum is zero.
    for (uint i = 0; i < IGNIS_N; ++i) {
        for (uint j = 0; j < IGNIS_DIM; ++j) {
            vel(j, i) -= pTot(j)/masses(i%N);
        }
    }

}

inline double getKineticEnergy(const MeshField * mf){

    double vVecSquared;
    double Ek = 0;

    const Ensemble* ensemble = mf->getEnsemble();

    const vec & masses = ensemble->masses;
    const uint & N = ensemble->nSpecies;

    for (const uint & k : mf->getAtoms()) {
        vVecSquared = ensemble->vel(0, k)*ensemble->vel(0, k)
                + ensemble->vel(1, k)*ensemble->vel(1, k);
#if IGNIS_DIM == 3
        vVecSquared += ensemble->vel(2, k)*ensemble->vel(2, k);
#endif
        Ek += masses(k%N)*vVecSquared;
    }

    return 0.5*Ek;

}

inline double getTemperature(MeshField * mf){

    return 2*getKineticEnergy(mf)/(3*mf->getPopulation());

}

} //End namespace gears

}

#endif // GEARS_H
