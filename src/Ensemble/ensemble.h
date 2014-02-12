#ifndef MD_ENSEMBLE_H
#define MD_ENSEMBLE_H

#include "../defines.h"

#include <armadillo>

using namespace arma;

namespace ignis
{
struct Ensemble
{

    mat::fixed<IGNIS_DIM, IGNIS_N> pos;
    mat::fixed<IGNIS_DIM, IGNIS_N> vel;
    mat::fixed<IGNIS_DIM, IGNIS_N> forces;
    cube::fixed<IGNIS_N, IGNIS_N, IGNIS_DIM> forceVectors;

    const vec masses;

    const int nSpecies;

    Ensemble(const vec & masses) : masses(masses), nSpecies(masses.n_elem) {}

};
}

#endif // MD_ENSEMBLE_H
