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

template<typename pT>
class MainMesh;

template<typename pT>
class MeshField
{

public:

    MeshField(const Mat<pT> & topology, const std::string description = "meshField");


    const pT volume;

    const typename Mat<pT>::template fixed<IGNIS_DIM, 2> topology;

    const typename Col<pT>::template fixed<IGNIS_DIM> shape;

    void setTopology(const Mat<pT> &topology, bool recursive=true);


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


    friend class MainMesh<pT>;
    friend class ContractMesh;
    friend class ExpandMesh;

protected:

    MeshField<pT> *parent;

    static PositionHandler<pT> &particles;

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


typedef MeshField<double> dField;

typedef MeshField<float>  fField;

typedef MeshField<uint>   uField;

typedef MeshField<int>    iField;


}

#include "meshfield.cpp"
