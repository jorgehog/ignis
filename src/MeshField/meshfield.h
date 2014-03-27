#ifndef MD_MESHFIELD_H
#define MD_MESHFIELD_H


#include "../defines.h"

#include <string>
#include <vector>
#include <exception>

#include <armadillo>

using namespace arma;


namespace ignis
{

struct Ensemble;

class Event;

class MainMesh;

class MeshField
{

protected:

    MeshField* parent;

    Ensemble *ensemble;

    std::vector<uint> atoms;

    std::vector<Event*> events;

    std::vector<MeshField*> subFields;


    virtual void sendToTop(Event & event);


    //This should be executed from the MainMesh,
    //As it recursively calls all subfields.
    void prepareEvents();


    bool append(uint i);

    bool checkSubFields(uint i);


    bool notCompatible(MeshField & subField);


    void resetSubFields();

    void resetContents(){
        atoms.clear();
    }

public:

    MeshField(const mat & topology, Ensemble &ensemble,
              const std::string description = "meshField");


    const double volume;

    const mat::fixed<IGNIS_DIM, 2> topology;

    const vec::fixed<IGNIS_DIM> shape;

    void setTopology(const mat & topology, bool recursive=true);


    const std::string description;


    virtual bool isMainMesh () const
    {
        return false;
    }

    void setParent(MeshField* parent) {
        this->parent = parent;
    }

    MeshField* getParent () {
        return parent;
    }


    virtual bool isWithinThis(uint i);

    void addEvent(Event & event);

    void removeEvent(uint i);

    void addSubField(MeshField &subField);

    void removeSubField(uint i){
        subFields.erase(subFields.begin() + i);
    }


    void stretchField(double l, uint xyz);

    void scaleField(const vec & oldShape, const mat &oldTopology, const mat &newTopology);


    virtual uint getPopulation() const {
        return atoms.size();
    }

    const std::vector<uint> & getAtoms() const {
        return atoms;
    }

    const Ensemble * getEnsemble () const {
        return ensemble;
    }


    const std::vector<MeshField*> & getSubfields() const {
        return subFields;
    }

    const std::vector<Event*> & getEvents() const {
        return events;
    }


    friend class MainMesh;
    friend class ContractMesh;
    friend class ExpandMesh;

};
}


#endif // MD_MESHFIELD_H
