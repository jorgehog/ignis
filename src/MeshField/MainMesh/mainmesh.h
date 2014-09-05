
#pragma once

#include "../meshfield.h"

namespace ignis
{

template<typename pT>
class MainMesh : public MeshField<pT>
{

public:

    static uint nCycles;

    MainMesh();

    MainMesh(const Mat<pT> &m_topology);

    MainMesh(const std::initializer_list<pT> m_topology);

    virtual ~MainMesh();

    void onConstruct();


    uint getPopulation() const;

    bool isMainMesh() const
    {
        return true;
    }

    static void setCurrentParticles(PositionHandler<pT> &particles)
    {
        m_currentParticles = &particles;
    }

    static void setCurrentParticles(PositionHandler<pT> *particles)
    {
        m_currentParticles = particles;
    }


    static PositionHandler<pT> * currentParticles()
    {
        return m_currentParticles;
    }

    void eventLoop();

    void setOutputPath(std::string path);

    void dumpEvents() const;

    void storeEventValues() const;

    std::string outputPath() const
    {
        return m_outputPath;
    }

    const std::string &filename()
    {
        return m_filename;
    }

    void enableProgressReport(const bool state)
    {
        m_reportProgress = state;
    }

    void enableOutput(const bool state, const uint outputSpacing = 1)
    {
        m_doOutput = state;

        m_outputSpacing = outputSpacing;
    }

    const uint &outputSpacing()
    {
        return m_outputSpacing;
    }

    void enableEventValueStorage(const bool storeInMatrix,
                                 const bool saveToFile,
                                 const std::string name = "ignisEventsOut.arma",
                                 const uint saveValuesSpacing = 1)
    {
        m_storeEventMatrix = storeInMatrix;

        m_storeEventMatrixToFile = saveToFile;

        m_filename = name;

        m_saveValuesSpacing = saveValuesSpacing;

    }

    const uint &saveValuesSpacing()
    {
        return m_saveValuesSpacing;
    }


private:

    void _sendToTop(Event<pT> &event);


    void _addIntrinsicEvents();


    void _sortEvents();

    void _initializeNewEvents();

    void _setupChunks();

    void _executeEvents();


    void _updateContainments();

    void _addIntrinsicEvent(Event<pT> *event)
    {
        this->addEvent(event);
        m_intrinsicEvents.push_back(event);
    }


    static PositionHandler<pT> *m_currentParticles;


    std::string m_outputPath;

    std::vector<Event<pT> *> m_allEvents;

    std::vector<Event<pT> *> m_intrinsicEvents;


    void _dumpLoopChunkInfo();

    bool m_doOutput;
    uint m_outputSpacing;

    bool m_storeEventMatrix;
    bool m_storeEventMatrixToFile;

    uint m_saveValuesSpacing;
    std::string m_filename;

    bool m_reportProgress;

    struct LoopChunk
    {

        uint m_start;
        uint m_end;

        std::vector<Event<pT> *> m_executeEvents;
        std::vector<Event<pT> *> m_resetEvents;

        LoopChunk(uint i, uint j) : m_start(i), m_end(j) {}

    };


    std::vector<LoopChunk *> m_allLoopChunks;

    LoopChunk * m_currentChunk;


};

typedef MainMesh<double> Mesh;
typedef MainMesh<float>  fMesh;
typedef MainMesh<int>    iLattice;
typedef MainMesh<uint>   Lattice;

}

#include "mainmesh.cpp"
