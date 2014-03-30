#include "meshfield.h"

using namespace ignis;

template<typename pT>
MeshField<pT>::MeshField(const Mat<pT> &topology, const std::string description):
    volume(0),
    description(description),
    particles(MainMesh<pT>::getCurrentParticles())
{
    setTopology(topology, false);
}


template<typename pT>
bool MeshField<pT>::isWithinThis(uint i) {

    for (uint j = 0; j < IGNIS_DIM; ++j) {
        if (particles(i, j) < topology(j, 0)){
            return false;
        } else if (particles(i, j) > topology(j, 1)) {
            return false;
        }
    }

    return true;

}

template<typename pT>
void MeshField<pT>::removeEvent(uint i)
{
    events.erase(events.begin() + i);

    for (uint j = i; j < events.size(); ++j)
    {
        events.at(j)->setAddress(j);
    }
}

template<typename pT>
void MeshField<pT>::resetSubFields()
{
    for (MeshField<pT> *subField : subFields)
    {
        subField->resetSubFields();
    }

    resetContents();
}

template<typename pT>
void MeshField<pT>::prepareEvents()
{

    for (Event<pT> *event : events)
    {
        event->setPriority();

        sendToTop(*event);

        event->setExplicitTimes();

    }

    for (MeshField<pT>* subfield : subFields)
    {
        subfield->prepareEvents();
    }

}


template<typename pT>
bool MeshField<pT>::append(uint i)
{

    if (isWithinThis(i)){
        atoms.push_back(i);
        return true;
    }

    return false;

}

template<typename pT>
void MeshField<pT>::sendToTop(Event<pT> &event)
{
    parent->sendToTop(event);
}

template<typename pT>
bool MeshField<pT>::checkSubFields(uint i)
{

    bool matchInSubField;
    bool matchedInSubLevel = false;

    for (MeshField<pT>* subField : subFields)
    {

        matchInSubField = subField->checkSubFields(i);
        matchedInSubLevel = matchedInSubLevel || matchInSubField;

    }

    if (matchedInSubLevel)
    {
        atoms.push_back(i);
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

    const mat & sft = subField.topology;
    const mat & tft = this->topology;

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
void MeshField<pT>::setTopology(const Mat<pT> &topology, bool recursive)
{

    if (recursive) {
        for (MeshField<pT> * subField : subFields) {
            subField->scaleField(shape, this->topology, topology);
        }
    }

    //Evil haxx for changing const values

    Mat<pT> * matPtr;
    matPtr = (Mat<pT>*)(&this->topology);
    *matPtr = topology;

    Col<pT> * vecPtr;
    vecPtr = (Col<pT>*)(&shape);
    *vecPtr = topology.col(1) - topology.col(0);

    //Calculate the volume
    double *new_volume;
    new_volume = (double*)(&volume);

    *new_volume = 1;
    for (uint i = 0; i < IGNIS_DIM; ++i) {
        *new_volume *= shape(i);
    }

}


template<typename pT>
void MeshField<pT>::addEvent(Event<pT> &event)
{

    event.setMeshField(this);

    event.setOutputVariables();

    events.push_back(&event);

    event.setAddress(events.size()-1);

}


template<typename pT>
void MeshField<pT>::addSubField(MeshField<pT>  & subField)
{

    if (notCompatible(subField)) {

        std::stringstream s;

        s << "subfield " << subField.description << " not compatible on " << description << std::endl;
        s << "CONFLICT:\nsubField\n" << subField.topology << " is out of bounds, similar to parent or inverted/empty\n" << topology << std::endl;

        Mat<pT> issue = (-topology + subField.topology);
        issue.col(1)*=-1;

        s << "Issue at negative or non-zero region:\n" << issue << std::endl;

        throw std::logic_error(s.str());

        return;
    }

    subField.setParent(this);
    subFields.push_back(&subField);

}

template<typename pT>
void MeshField<pT>::stretchField(double deltaL, uint xyz)
{

    Mat<pT> newTopology = topology;
    newTopology(xyz, 0) += deltaL/2;
    newTopology(xyz, 1) -= deltaL/2;

    setTopology(newTopology);

}

template<typename pT>
void MeshField<pT>::scaleField(const Col<pT> & oldShape, const Mat<pT> & oldTopology, const Mat<pT> & newTopology){

    Mat<pT> newSubTopology(IGNIS_DIM, 2);

    double oldCOM, newCOM, shapeFac, newShape_i, newSubShape_i;

    for (uint i = 0; i < IGNIS_DIM; ++i) {

        newShape_i = newTopology(i, 1) - newTopology(i, 0);
        shapeFac = newShape_i/oldShape(i);

        newSubShape_i = shapeFac*shape(i);

        oldCOM = topology(i, 1) - shape(i)/2 - oldTopology(i, 0);

        newCOM = shapeFac*oldCOM;

        newSubTopology(i, 0) = newTopology(i, 0) + newCOM - newSubShape_i/2;
        newSubTopology(i, 1) = newTopology(i, 0) + newCOM + newSubShape_i/2;

    }

    setTopology(newSubTopology);

}
