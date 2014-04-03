#pragma once

#include "../meshfield.h"

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

    static PositionHandler<pT> * currentParticles()
    {
        return m_currentParticles;
    }

    void eventLoop(uint N);

    void setOutputPath(std::string path);

    void dumpEvents() const;

    void dumpEventsToFile() const;

    void enableProgressReport(const bool state)
    {
        m_reportProgress = state;
    }

    static void enableOutput(const bool state)
    {
        m_doOutput = state;
    }

    void enableEventFile(const bool state)
    {
        m_doFileIO = state;
    }


private:

    void sendToTop(Event<pT> &event);


    void addIntrinsicEvents();


    void sortEvents();

    void initializeNewEvents();

    void setupChunks();

    void executeEvents();


    void updateContainments();


    static PositionHandler<pT> *m_currentParticles;


    std::string outputPath;

    std::vector<Event<pT> *> allEvents;


    void dumpLoopChunkInfo();

    static bool m_doOutput;

    static bool m_doFileIO;

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

typedef MainMesh<uint>  MainLattice;
typedef MainMesh<int>   iMainLattice;
typedef MainMesh<float> fMainLattice;

}

#include "mainmesh.cpp"
