#pragma once

#include "../defines.h"

#include <string>
#include <vector>


#include <armadillo>

using namespace arma;


namespace ignis
{

template<typename pT>
class PositionHandler;

template<typename pT = double>
class Event;

template<typename pT = double>
class MainMesh;

template<typename pT>
class MeshField
{
public:

    typedef pT type;
    typedef typename Mat<pT>::template fixed<IGNIS_DIM, 2> topmat;
    typedef typename Col<pT>::template fixed<IGNIS_DIM>    shapevec;

    MeshField(const std::string description);

    MeshField(const topmat & topology, const std::string description = "meshField");

    MeshField(const std::initializer_list<pT> topology, const std::string description);


    const pT volume;

    const topmat topology;

    const shapevec shape;

    void setTopology(const topmat &topology, bool recursive=true);

    void setTopology(const std::initializer_list<pT> topology, bool recursive=true);


    const std::string description;


    virtual bool isMainMesh () const
    {
        return false;
    }

    void setParent(MeshField<pT> *parent)
    {
        this->parent = parent;
    }

    MeshField<pT> *getParent ()
    {
        return parent;
    }


    virtual bool isWithinThis(uint i);

    void addEvent(Event<pT> & event);

    void removeEvent(uint i);

    void addSubField(MeshField &subField);

    void removeSubField(uint i)
    {
        subFields.erase(subFields.begin() + i);
    }


    void stretchField(double l, uint xyz);

    void scaleField(const Col<pT> &oldShape, const Mat<pT> &oldTopology, const Mat<pT> &newTopology);


    virtual uint getPopulation() const
    {
        return atoms.size();
    }

    const std::vector<uint> & getAtoms() const
    {
        return atoms;
    }


    const std::vector<MeshField*> & getSubfields() const
    {
        return subFields;
    }

    const std::vector<Event<pT> *> & getEvents() const
    {
        return events;
    }


    uint totalNumberOfParticles() const
    {
        return m_particles->count();
    }

    friend class MainMesh<pT>;

protected:

    PositionHandler<pT> *m_particles;

    MeshField<pT> *parent;


    const pT &particles(const uint n, const uint d)
    {
        return (*m_particles)(n, d);
    }

    std::vector<uint> atoms;

    std::vector<Event<pT>* > events;

    std::vector<MeshField<pT>* > subFields;


    virtual void sendToTop(Event<pT> & event);


    //This should be executed from the MainMesh,
    //As it recursively calls all subfields.
    void prepareEvents();

    bool append(uint i);

    bool checkSubFields(uint i);


    bool notCompatible(MeshField<pT> & subField);

    void resetSubFields();

    void resetContents()
    {
        atoms.clear();
    }

};


typedef MeshField<double> meshfield;

typedef MeshField<float>  fmeshfield;

typedef MeshField<uint>   lattice;

typedef MeshField<int>    ilattice;


}

#include "meshfield.cpp"
