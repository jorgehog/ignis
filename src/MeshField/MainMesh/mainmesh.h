#ifndef MAINMESH_H
#define MAINMESH_H

#include "../meshfield.h"

namespace ignis
{

struct LoopChunk
{

    uint start;
    uint end;

    std::vector<Event*> executeEvents;
    std::vector<Event*> resetEvents;

    LoopChunk(uint i, uint j) : start(i), end(j) {}

};

class MainMesh : public MeshField
{

public:
    MainMesh(const mat & topology, Particles & particles);

    uint getPopulation() const {
        return IGNIS_N;
    }

    bool isMainMesh() const
    {
        return true;
    }


    void eventLoop(uint N);

    void setOutputPath(std::string path);

    void dumpEvents() const;

    void dumpEventsToFile() const;



    void setSilent(const bool state)
    {
        m_silent = state;
    }

    void setFileIOState(const bool state)
    {
        m_doFileIO = state;
    }


private:

    void sendToTop(Event &event);


    void addIntrinsicEvents();


    void sortEvents();

    void initializeNewEvents();

    void setupChunks();

    void executeEvents();



    void updateContainments();


    std::string outputPath;

    std::vector<Event*> allEvents;

    std::vector<LoopChunk*> allLoopChunks;

    LoopChunk * currentChunk;

    void dumpLoopChunkInfo();

    bool m_silent;

    bool m_doFileIO;


};
}

#endif // MAINMESH_H
