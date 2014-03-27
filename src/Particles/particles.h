#pragma once

#include "../defines.h"

#include <armadillo>

using namespace arma;

namespace ignis
{

struct Particles
{

    mat pos;
    mat vel;
    mat forces;
    cube forceVectors;

    const vec masses;

    const uint & nSpecies() const
    {
        return masses.n_elem;
    }

    Particles(const vec & masses) : masses(masses) {}

};

}

