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

    uint NX;
    uint NY;
    uint NZ;
    uint N;

    const vec masses;

    const uint & nSpecies() const
    {
        return masses.n_elem;
    }

    uint count() const
    {
        return N;
    }

    Particles(const vec & masses, const uint NX, const uint NY, const uint NZ = 0) :
        NX(NX),
        NY(NY),
        NZ(NZ),
        N(NZ == 0 ? NX*NY : NX*NY*NZ),
        masses(masses)
    {
        pos.set_size(IGNIS_DIM, N);
        vel.set_size(IGNIS_DIM, N);
        forces.set_size(IGNIS_DIM, N);
        forceVectors.set_size(N, N, IGNIS_DIM);
    }

};

}

