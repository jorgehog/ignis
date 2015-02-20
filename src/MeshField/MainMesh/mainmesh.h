
#pragma once

#include "../meshfield.h"

#include <fstream>

namespace ignis
{

template<typename pT>
class _particleHandler;

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

    void dumpLoopChunkInfo();

    void terminateLoop(std::string terminateMessage = "", std::string terminator = "Unknown")
    {
        m_terminate = true;
        m_terminateMessage = terminateMessage;
        m_terminator = terminator;
    }

    void _terminate(const uint cycle, const uint nCycles)
    {
        cout << "ignis::eventLoop terminated by "
             << m_terminator
             << " at cycle "
             << cycle << " / " << nCycles;

        if (!m_terminateMessage.empty())
        {
            cout << ". What: " << m_terminateMessage << ".";
        }

        cout << endl;

        if (m_storeEvents)
        {
            m_storedEventValues.resize(cycle/saveValuesSpacing(), numberOfStoredEvents());
        }
    }

    friend class _particleHandler<pT>;

private:

    static PositionHandler<pT> *m_currentParticles;

    mat m_storedEventValues;

    std::vector<std::string> m_storedEventTypes;

    std::ofstream m_eventStorageFile;

    std::string m_outputPath;

    std::vector<Event<pT> *> m_allEvents;

    std::vector<Event<pT> *> m_intrinsicEvents;

    std::vector<Event<pT> *> m_storageEnabledEvents;

    bool m_handleParticles;

    bool m_doOutput;
    uint m_outputSpacing;

    bool m_storeEvents;
    bool m_storeEventsToFile;

    uint m_saveValuesSpacing;
    std::string m_filename;

    bool m_reportProgress;

    bool m_terminate;
    std::string m_terminateMessage;
    std::string m_terminator;

    struct LoopChunk
    {

        uint m_start;
        uint m_end;

        std::vector<Event<pT> *> m_events;

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
