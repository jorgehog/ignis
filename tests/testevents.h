#pragma once

#include <ignis.h>
#include "testsetup.h"

namespace ignis
{

class SetAndGet : public TestEvent
{
    using TestEvent::TestEvent;


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

}
