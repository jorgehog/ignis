#pragma once

#include <ignis.h>
#include "testsetup.h"

namespace ignis
{

class SetAndGet : public MeshEvent
{
    using MeshEvent::MeshEvent;


    // Event interface
public:
    void initialize()
    {
        for (uint i = 0; i < totalNumberOfParticles(); ++i)
        {
            for (uint j = 0; j < IGNIS_DIM; ++j)
            {
                registeredHandler(i, j) = (j + 1)*(i + 1);
            }
        }
    }

protected:
    void execute()
    {

        double p = 0;

        for (uint i = 0; i < totalNumberOfParticles(); ++i)
        {
            for (uint j = 0; j < IGNIS_DIM; ++j)
            {
                p += registeredHandler(i, j);
            }
        }

        setValue(p);

    }
};

class SaveData : public MeshEvent
{
public:

    SaveData(const double f) :
        MeshEvent("SaveData", "", false, true),
        m_f(f)
    {

    }

private:

    const double m_f;

    // Event interface
protected:
    void execute()
    {
        setValue(m_f*cycle());
    }
};

}
