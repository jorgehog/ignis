#pragma once

#include <ignis.h>


namespace ignis
{

class TestSystem : public PositionHandler<double>
{
public:

    virtual double operator() (const uint n, const uint d) const
    {
        return data[n][d];
    }

    virtual double &operator() (const uint n, const uint d)
    {
        return data[n][d];
    }

private:

    double data[30][IGNIS_DIM];


    // PositionHandler interface
public:
    uint count() const
    {
        return 30;
    }
};

class TestEvent : public MeshEvent
{
    using MeshEvent::MeshEvent;
};

}
