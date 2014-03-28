#pragma once

#include <armadillo>

namespace ignis
{

template<typename pT>
class PositionHandler
{

    typedef typename arma::Col<pT>::template fixed<IGNIS_DIM> colType;


public:



    virtual uint count() const = 0;

    virtual pT &at(const uint n, const uint d) const = 0;



    pT &operator()(const uint n, const uint d) const
    {
        return at(n, d);
    }

    colType &operator()(const uint n) const
    {
        colType* _col = new colType;

        _col->memptr()[0] = this->at(n, 0);
        _col->memptr()[1] = this->at(n, 1);
#if INGIS_DIM == 3
        _col->memptr()[2] = this->at(n, 2);
#endif

        return *_col;

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
