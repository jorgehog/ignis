#pragma once

#include "../defines.h"

#include <string>
#include <vector>

#include <armadillo>

#include <BADAss/badass.h>


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

    virtual ~MeshField();

    const pT volume;

    const topmat topology;

    const shapevec shape;

    void setTopology(const topmat &topology, bool recursive=true);

    void setTopology(const std::initializer_list<pT> topology, bool recursive=true);


    virtual bool isMainMesh () const
    {
        return false;
    }

    void setParent(MeshField<pT> *parent)
    {
        this->m_parent = parent;
    }

    MeshField<pT> *getParent ()
    {
        return m_parent;
    }


    virtual bool isWithinThis(uint i);

    void addEvent(Event<pT> & event);

    void addEvent(Event<pT> * event)
    {
        addEvent(*event);
    }

    bool hasEvent(const Event<pT> *event) const
    {
        return std::find(m_events.begin(), m_events.end(), event) != m_events.end();
    }

    void removeEvent(uint i);

    void removeEvent(const Event<pT> *event);

    void addSubField(MeshField &subField);

    void addSubField(MeshField *subField)
    {
        addSubField(*subField);
    }

    void removeSubField(uint i)
    {
        m_subFields.erase(m_subFields.begin() + i);
    }


    void stretchField(double l, uint xyz);

    void scaleField(const Col<pT> &oldShape, const topmat &oldTopology, const topmat &newTopology);


    const string description() const
    {
        return m_description;
    }

    virtual uint getPopulation() const
    {
        return m_atoms.size();
    }

    const std::vector<uint> & getAtoms() const
    {
        return m_atoms;
    }


    const std::vector<MeshField*> & getSubfields() const
    {
        return m_subFields;
    }

    const std::vector<Event<pT> *> & getEvents() const
    {
        return m_events;
    }


    uint totalNumberOfParticles() const
    {
        return m_particles->count();
    }

    virtual void terminateLoop(std::string terminateMessage = "", std::string terminator = "Unknown");


    friend class MainMesh<pT>;

protected:

    const std::string m_description;

    PositionHandler<pT> *m_particles;

    MeshField<pT> *m_parent;


    const pT &particles(const uint n, const uint d)
    {
        return (*m_particles)(n, d);
    }

    std::vector<uint> m_atoms;

    std::vector<Event<pT>* > m_events;

    std::vector<MeshField<pT>* > m_subFields;


    void _prepareEvent(Event<pT> *event, const uint nCycles, const uint *loopCyclePtr);

    virtual void _sendToTop(Event<pT> & event);


    //This should be executed from the MainMesh,
    //As it recursively calls all subfields.
    void _prepareEvents(const uint nCycles, const uint *loopCyclePtr);

    bool append(uint i);

    bool checkSubFields(uint i);


    bool notCompatible(MeshField<pT> & subField);

    void resetSubFields();

    void resetContents()
    {
        m_atoms.clear();
    }

};


typedef MeshField<double> meshfield;
typedef MeshField<float>  fmeshfield;
typedef MeshField<uint>   latticefield;
typedef MeshField<int>    ilatticefield;


}

#include "meshfield.cpp"
