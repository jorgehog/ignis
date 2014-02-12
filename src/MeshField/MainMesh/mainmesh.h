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
    MainMesh(const mat & topology, Ensemble &ensemble);

    uint getPopulation() const {
        return IGNIS_N;
    }

    void eventLoop(uint N);

    void setOutputPath(std::string path);

    void dumpEvents() const;

    void dumpEventsToFile() const;


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



};
}

#endif // MAINMESH_H
