#include <ignis.h>

#include "testsetup.h"

#include "testevents.h"

#include <unittest++/UnitTest++.h>

using namespace ignis;

double factorial(int N)
{
    if (N == 0)
    {
        return 1;
    }

    return N*factorial(N - 1);
}

TEST(setAndGet)
{
    TestSystem system;

    Mesh::setCurrentParticles(&system);

    Mesh mesh = {10, 10 , 10};

    SetAndGet event1;

    mesh.addEvent(event1);

    mesh.nCycles = 5;

    mesh.eventLoop();

    const uint D = IGNIS_DIM;
    const uint N = system.count();

    double CF = D*(D+1)/2*N*(N+1)/2;

    CHECK_CLOSE(CF, event1.getMeasurement(), 1E-1);

}


int main()
{

    Mesh::enableOutput(false);

    return UnitTest::RunAllTests();

}
