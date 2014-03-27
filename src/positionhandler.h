#pragma once

#include <armadillo>

namespace ignis
{

template<typename pT>
class PositionHandler
{
public:



    virtual uint count() const = 0;

    virtual pT &at(const uint n, const uint d) const = 0;



    pT &operator()(const uint n, const uint d) const
    {
        return at(n, d);
    }

    arma::Col<pT> &operator()(const uint n) const
    {
        pT *aux_mem[IGNIS_DIM];

        aux_mem[0] = this->at(n, 0);
        aux_mem[1] = this->at(n, 1);
#if INGIS_DIM == 3
        aux_mem[2] = this->at(n, 2);
#endif

        return arma::Col<pT>::fixed<IGNIS_DIM>(aux_mem, false, true);

    }

    operator arma::Mat<pT> () const
    {

        arma::Mat<pT> m(count(), IGNIS_DIM);

        for(uint i = 0; i < count(); ++i)
        {
            for (uint j = 0; j < IGNIS_DIM; ++j)
            {
                m(i, j) = at(i, j);
            }
        }

        return m;
    }


};

template<typename pT>
class DummyHandler : public PositionHandler<pT>
{
private:

    pT* data[10][IGNIS_DIM];

    // PositionHandler interface
public:
    uint count() const
    {
        return 10;
    }

    pT &at(const uint n, const uint d) const
    {
        return *data[n][d];
    }
};


}
