#include "meshfield.h"

#include "MainMesh/mainmesh.h"

using namespace ignis;

template<typename pT>
MeshField<pT>::MeshField(const std::string description) :
    m_volume(0),
    m_description(description),
    m_particles(MainMesh<pT>::currentParticles())
{
    topmat *_top = new topmat(fill::zeros);
    setTopology(*_top);
}

template<typename pT>
MeshField<pT>::MeshField(const topmat &topology, const std::string description) :
    m_volume(0),
    m_description(description),
    m_particles(MainMesh<pT>::currentParticles())
{
    setTopology(topology, false);
}

template<typename pT>
MeshField<pT>::MeshField(const std::initializer_list<pT> topology, const std::string description) :
    m_volume(0),
    m_description(description),
    m_particles(MainMesh<pT>::currentParticles())
{
    setTopology(topology);
}

template<typename pT>
MeshField<pT>::~MeshField()
{
    for (MeshField<pT> *meshField : m_subFields)
    {
        delete meshField;
    }
}


template<typename pT>
bool MeshField<pT>::isWithinThis(uint i) {

    for (uint j = 0; j < IGNIS_DIM; ++j) {
        if (particles(i, j) < m_topology(j, 0)){
            return false;
        } else if (particles(i, j) > m_topology(j, 1)) {
            return false;
        }
    }

    return true;

}



template<typename pT>
void MeshField<pT>::removeEvent(uint i)
{
    m_events.erase(m_events.begin() + i);

    for (uint j = i; j < m_events.size(); ++j)
    {
        m_events.at(j)->setAddress(j);
    }
}

template<typename pT>
void MeshField<pT>::resetSubFields()
{
    for (MeshField<pT> *subField : m_subFields)
    {
        subField->resetSubFields();
    }

    resetContents();
}

template<typename pT>
void MeshField<pT>::prepareEvents()
{

    for (Event<pT> *event : m_events)
    {
        event->setPriority();

        _sendToTop(*event);

        event->setExplicitTimes();

    }

    for (MeshField<pT>* subfield : m_subFields)
    {
        subfield->prepareEvents();
    }

}


template<typename pT>
bool MeshField<pT>::append(uint i)
{

    if (isWithinThis(i)){
        m_atoms.push_back(i);
        return true;
    }

    return false;

}

template<typename pT>
void MeshField<pT>::_sendToTop(Event<pT> &event)
{
    m_parent->_sendToTop(event);
}

template<typename pT>
bool MeshField<pT>::checkSubFields(uint i)
{

    bool matchInSubField;
    bool matchedInSubLevel = false;

    for (MeshField<pT>* subField : m_subFields)
    {

        matchInSubField = subField->checkSubFields(i);
        matchedInSubLevel = matchedInSubLevel || matchInSubField;

    }

    if (matchedInSubLevel)
    {
        m_atoms.push_back(i);
    }
    else
    {
        matchedInSubLevel = append(i);
    }

    return matchedInSubLevel;

}



template<typename pT>
bool MeshField<pT>::notCompatible(MeshField<pT> &subField)
{

    if (&subField == this) return true;

    const topmat & sft = subField.m_topology;
    const topmat & tft = this->m_topology;

#if IGNIS_DIM == 2
    bool outsideMesh =  (sft(0, 0) < tft(0, 0)) ||
            (sft(0, 1) > tft(0, 1)) ||
            (sft(1, 0) < tft(1, 0)) ||
            (sft(1, 1) > tft(1, 1));

    bool equalMesh   =  (sft(0, 0) == tft(0, 0)) &&
            (sft(0, 1) == tft(0, 1)) &&
            (sft(1, 0) == tft(1, 0)) &&
            (sft(1, 1) == tft(1, 1));
    bool inverted    =  (sft(0, 0) >= sft(0, 1)) ||
            (sft(1, 0) >= sft(1, 1));
#elif IGNIS_DIM == 3
    bool outsideMesh =  (sft(0, 0) < tft(0, 0)) ||
            (sft(0, 1) > tft(0, 1)) ||
            (sft(1, 0) < tft(1, 0)) ||
            (sft(1, 1) > tft(1, 1)) ||
            (sft(2, 0) < tft(2, 0)) ||
            (sft(2, 1) > tft(2, 1));

    bool equalMesh   =  (sft(0, 0) == tft(0, 0)) &&
            (sft(1, 0) == tft(1, 0)) &&
            (sft(0, 1) == tft(0, 1)) &&
            (sft(1, 1) == tft(1, 1)) &&
            (sft(0, 2) == tft(0, 2)) &&
            (sft(1, 2) == tft(1, 2));

    bool inverted    =  (sft(0, 0) >= sft(0, 1)) ||
            (sft(1, 0) >= sft(1, 1)) ||
            (sft(2, 0) >= sft(2, 1));
#endif

    return outsideMesh || equalMesh || inverted;

}

template<typename pT>
void MeshField<pT>::setTopology(const topmat &topology, bool recursive)
{

    if (recursive) {
        for (MeshField<pT> * subField : m_subFields) {
            subField->scaleField(m_shape, this->m_topology, topology);
        }
    }

    //Evil haxx for changing const values

    topmat * matPtr;
    matPtr = (topmat*)(&this->m_topology);
    *matPtr = topology;

    shapevec * vecPtr;
    vecPtr = (shapevec*)(&m_shape);
    *vecPtr = topology.col(1) - topology.col(0);

    //Calculate the volume
    pT *new_volume;
    new_volume = (pT*)(&m_volume);

    *new_volume = 1;
    for (uint i = 0; i < IGNIS_DIM; ++i) {
        *new_volume *= m_shape(i);
    }

}

template<typename pT>
void MeshField<pT>::setTopology(const std::initializer_list<pT> topology, bool recursive)
{
    const topmat *newTop = new topmat(topology);

    setTopology(*newTop, recursive);

    delete newTop;

}


template<typename pT>
void MeshField<pT>::addEvent(Event<pT> &event)
{

    event.setMeshField(this);

    event.setOutputVariables();

    m_events.push_back(&event);

    event.setAddress(m_events.size()-1);

}


template<typename pT>
void MeshField<pT>::addSubField(MeshField<pT>  & subField)
{

    if (notCompatible(subField)) {

        std::stringstream s;

        s << "subfield " << subField.m_description << " not compatible on " << m_description << std::endl;
        s << "CONFLICT:\nsubField\n" << subField.m_topology << " is out of bounds, similar to parent or inverted/empty\n" << m_topology << std::endl;

        Mat<pT> issue = (-m_topology + subField.m_topology);
        issue.col(1)*=-1;

        s << "Issue at negative or non-zero region:\n" << issue << std::endl;

        throw std::logic_error(s.str());

        return;
    }

    subField.setParent(this);
    m_subFields.push_back(&subField);

}

template<typename pT>
void MeshField<pT>::stretchField(double deltaL, uint xyz)
{

    Mat<pT> newTopology = m_topology;
    newTopology(xyz, 0) += deltaL/2;
    newTopology(xyz, 1) -= deltaL/2;

    setTopology(newTopology);

}

template<typename pT>
void MeshField<pT>::scaleField(const Col<pT> & oldShape, const topmat &oldTopology, const topmat &newTopology){

    Mat<pT> newSubTopology(IGNIS_DIM, 2);

    double oldCOM, newCOM, shapeFac, newShape_i, newSubShape_i;

    for (uint i = 0; i < IGNIS_DIM; ++i) {

        newShape_i = newTopology(i, 1) - newTopology(i, 0);
        shapeFac = newShape_i/oldShape(i);

        newSubShape_i = shapeFac*m_shape(i);

        oldCOM = m_topology(i, 1) - m_shape(i)/2 - oldTopology(i, 0);

        newCOM = shapeFac*oldCOM;

        newSubTopology(i, 0) = newTopology(i, 0) + newCOM - newSubShape_i/2;
        newSubTopology(i, 1) = newTopology(i, 0) + newCOM + newSubShape_i/2;

    }

    setTopology(newSubTopology);

}
