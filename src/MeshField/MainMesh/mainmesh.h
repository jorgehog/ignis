
#pragma once

#include "../meshfield.h"

#include <fstream>

namespace ignis
{

template<typename pT>
class MainMesh : public MeshField<pT>
{

public:

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

    void eventLoop(const uint nCycles);

    void setOutputPath(std::string path);

    void dumpEvents() const;

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

    void enableEventValueStorage(const bool store,
                                 const bool saveToFile,
                                 const std::string name = "ignisEventsOut.arma",
                                 const std::string path = "/tmp",
                                 const uint saveValuesSpacing = 1)
    {
        m_storeEvents = store;

        m_storeEventsToFile = saveToFile;

        m_filename = name;

        setOutputPath(path);

        m_saveValuesSpacing = saveValuesSpacing;

    }

    const uint &saveValuesSpacing()
    {
        return m_saveValuesSpacing;
    }

    uint numberOfStoredEvents() const
    {
        return m_storageEnabledEvents.size();
    }


    void _initializeEventStorage(const uint size);

    void _storeEventValues(const uint index);


private:

    mat m_storedEventValues;

    std::vector<std::string> m_storedEventTypes;

    std::ofstream m_eventStorageFile;

    const std::vector<std::string> &outputEventDescriptions()
    {
        return m_storedEventTypes;
    }

    const mat &storedEventValues()
    {
        return m_storedEventValues;
    }


    void dumpStoredEvent(uint k)
    {
        for (uint i = 0; i < numberOfStoredEvents(); ++i)
        {
            cout << std::setw(30) << std::left << m_storedEventTypes.at(i) << "  " << std::setw(10) << m_storedEventValues(k, i) << endl;
        }
    }



    static PositionHandler<pT> *m_currentParticles;


    std::string m_outputPath;

    std::vector<Event<pT> *> m_allEvents;

    std::vector<Event<pT> *> m_intrinsicEvents;

    std::vector<Event<pT> *> m_storageEnabledEvents;


    void _dumpLoopChunkInfo();

    bool m_doOutput;
    uint m_outputSpacing;

    bool m_storeEvents;
    bool m_storeEventsToFile;

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

    void _streamValueToFile(const double value);

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

    void _finalize();


};

typedef MainMesh<double> Mesh;
typedef MainMesh<float>  fMesh;
typedef MainMesh<int>    iLattice;
typedef MainMesh<uint>   Lattice;

}

#include "mainmesh.cpp"
