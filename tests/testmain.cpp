#include <ignis.h>

#include "testsetup.h"

#include "testevents.h"

#include <unittest++/UnitTest++.h>

using namespace ignis;
using namespace std;

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
    mesh.enableOutput(false);


    SetAndGet event1;

    mesh.addEvent(event1);

    mesh.nCycles = 5;

    mesh.eventLoop();

    const uint D = IGNIS_DIM;
    const uint N = system.count();

    double CF = D*(D+1)/2*N*(N+1)/2;

    CHECK_CLOSE(CF, event1.getMeasurement(), 1E-1);

}


TEST(saveAndLoad)
{
    TestSystem system;
    Mesh::setCurrentParticles(system);

    Mesh mesh = {10, 10 , 10};

    mesh.enableOutput(false);
    mesh.enableEventValueStorage(true, true, "test.arma");

    uint K = 10;
    vector<SaveData*> saveDataEvents(K);
    for (uint i = 0; i < K; ++i)
    {
        SaveData *saveDataEvent = new SaveData(i + 1);

        mesh.addEvent(saveDataEvent);
        saveDataEvents.push_back(saveDataEvent);

    }

    mesh.eventLoop();

    mat loadMatrix;
    loadMatrix.load(mesh.outputPath() + "test.arma");

    for (uint i = 0; i < mesh.nCycles; ++i)
    {
        for (uint k = 0; k < K; ++k)
        {
            CHECK_EQUAL(loadMatrix(i, k), (k + 1)*i);
        }
    }

    for (SaveData* event : saveDataEvents)
    {
        delete event;
    }



}

int main()
{
    return UnitTest::RunAllTests();
}
