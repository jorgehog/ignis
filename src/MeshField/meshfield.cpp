#include "meshfield.h"

#include <sstream>
#include "../Ensemble/ensemble.h"
#include "../Event/event.h"

using namespace ignis;

MeshField::MeshField(const mat &topology, Ensemble  & ensemble, const std::string description):
    m_isMainMesh(false),
    volume(0),
    description(description)
{
    this->ensemble = &ensemble;

    setTopology(topology, false);

}


bool MeshField::isWithinThis(uint i) {

    for (uint j = 0; j < IGNIS_DIM; ++j) {
        if (ensemble->pos(j, i) < topology(j, 0)){
            return false;
        } else if (ensemble->pos(j, i) > topology(j, 1)) {
            return false;
        }
    }

    return true;

}

void MeshField::removeEvent(uint i)
{
    events.erase(events.begin() + i);

    for (uint j = i; j < events.size(); ++j) {
        events.at(j)->setAddress(j);
    }
}


void MeshField::resetSubFields()
{
    for (MeshField* subField : subFields){
        subField->resetSubFields();
    }

    resetContents();
}

void MeshField::prepareEvents()
{

    for (Event* event : events)
    {
        event->setPriority();

        sendToTop(*event);

        event->setExplicitTimes();

    }

    for (MeshField* subfield : subFields)
    {
        subfield->prepareEvents();
    }

}

bool MeshField::append(uint i)
{

    if (isWithinThis(i)){
        atoms.push_back(i);
        return true;
    }

    return false;

}


void MeshField::sendToTop(Event &event)
{
    parent->sendToTop(event);
}


bool MeshField::checkSubFields(uint i)
{

    bool matchInSubField;
    bool matchedInSubLevel = false;

    for (MeshField* subField : subFields)
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



bool MeshField::notCompatible(MeshField & subField)
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

void MeshField::setTopology(const mat &topology, bool recursive)
{

    if (recursive) {
        for (MeshField * subField : subFields) {
            subField->scaleField(shape, this->topology, topology);
        }
    }

    //Evil haxx for changing const values

    mat * matPtr;
    matPtr = (mat*)(&this->topology);
    *matPtr = topology;

    vec * vecPtr;
    vecPtr = (vec*)(&shape);
    *vecPtr = topology.col(1) - topology.col(0);

    //Calculate the volume
    double *new_volume;
    new_volume = (double*)(&volume);

    *new_volume = 1;
    for (uint i = 0; i < IGNIS_DIM; ++i) {
        *new_volume *= shape(i);
    }

}


void MeshField::addEvent(Event & event)
{

    event.setMeshField(this);

    event.setOutputVariables();

    event.setEnsemble(ensemble);

    events.push_back(&event);

    event.setAddress(events.size()-1);

}

void MeshField::addSubField(MeshField  & subField)
{

    if (notCompatible(subField)) {
        std::cout << "subfield " << subField.description << " not compatible on " << description << std::endl;
        std::cout << "CONFLICT:\nsubField\n" << subField.topology << " is out of bounds, similar to parent or inverted/empty\n" << topology << std::endl;
        mat issue = (-topology + subField.topology);
        issue.col(1)*=-1;

        std::cout << "Issue at negative or non-zero region:\n" << issue << std::endl;
        throw *meshException;
        return;
    }

    subField.setParent(this);
    subFields.push_back(&subField);

}

void MeshField::stretchField(double deltaL, uint xyz)
{

    mat newTopology = topology;
    newTopology(xyz, 0) += deltaL/2;
    newTopology(xyz, 1) -= deltaL/2;

    setTopology(newTopology);

}

void MeshField::scaleField(const vec & oldShape, const mat & oldTopology, const mat & newTopology){

    mat newSubTopology(IGNIS_DIM, 2);

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
