#pragma once

#include "../defines.h"

#include <armadillo>

using namespace arma;

namespace ignis
{

struct Particles
{

    mat::fixed<IGNIS_DIM, IGNIS_N> pos;
    mat::fixed<IGNIS_DIM, IGNIS_N> vel;
    mat::fixed<IGNIS_DIM, IGNIS_N> forces;
    cube::fixed<IGNIS_N, IGNIS_N, IGNIS_DIM> forceVectors;

    const vec masses;

    const uint & nSpecies() const
    {
        return masses.n_elem;
    }

    Particles(const vec & masses) : masses(masses) {}

};

}

