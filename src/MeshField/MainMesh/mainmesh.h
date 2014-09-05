
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

    MainMesh(const Mat<pT> &topology);

    MainMesh(const std::initializer_list<pT> topology);

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

    static const std::string &filename()
    {
        return m_filename;
    }

    static void enableProgressReport(const bool state)
    {
        m_reportProgress = state;
    }

    static void enableOutput(const bool state, const uint outputSpacing = 1)
    {
        m_doOutput = state;

        m_outputSpacing = outputSpacing;
    }

    static const uint &outputSpacing()
    {
        return m_outputSpacing;
    }

    static void enableEventFile(const bool state,
                                const std::string name = "ignisEventsOut.arma",
                                const uint saveFileSpacing = 1000,
                                const uint saveValuesSpacing = 1)
    {
        m_doFileIO = state;

        m_filename = name;

        m_saveValuesSpacing = saveValuesSpacing;

        m_saveFileSpacing = saveFileSpacing;

    }

    static const uint &saveValuesSpacing()
    {
        return m_saveValuesSpacing;
    }

    static const uint &saveFileSpacing()
    {
        return m_saveFileSpacing;
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

    std::vector<Event<pT> *> allEvents;

    std::vector<Event<pT> *> m_intrinsicEvents;


    void _dumpLoopChunkInfo();

    static bool m_doOutput;
    static uint m_outputSpacing;

    static bool m_doFileIO;
    static uint m_saveValuesSpacing;
    static uint m_saveFileSpacing;
    static std::string m_filename;

    static bool m_reportProgress;

    struct LoopChunk
    {

        uint start;
        uint end;

        std::vector<Event<pT> *> executeEvents;
        std::vector<Event<pT> *> resetEvents;

        LoopChunk(uint i, uint j) : start(i), end(j) {}

    };


    std::vector<LoopChunk *> allLoopChunks;

    LoopChunk * currentChunk;


};

typedef MainMesh<double> Mesh;
typedef MainMesh<float>  fMesh;
typedef MainMesh<int>    iLattice;
typedef MainMesh<uint>   Lattice;

}

#include "mainmesh.cpp"
