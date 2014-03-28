#pragma once

#include "../meshfield.h"

namespace ignis
{

template<typename pT = double> class MainMesh;

template<typename pT>
class MainMesh : public MeshField<pT>
{

public:

    MainMesh(const Mat<pT> &topology);

    uint getPopulation() const;

    bool isMainMesh() const
    {
        return true;
    }

    static void setCurrentParticles(const PositionHandler<pT> &particles)
    {
        m_currentParticles = &particles;
    }

    static const PositionHandler<pT> & getCurrentParticles()
    {
        return *m_currentParticles;
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

    void sendToTop(Event<pT> &event);


    void addIntrinsicEvents();


    void sortEvents();

    void initializeNewEvents();

    void setupChunks();

    void executeEvents();


    void updateContainments();


    static const PositionHandler<pT> *m_currentParticles;


    std::string outputPath;

    std::vector<Event<pT> *> allEvents;


    void dumpLoopChunkInfo();

    bool m_silent;

    bool m_doFileIO;



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




    class _dumpEvents : public Event<pT>
    {
    public:

        _dumpEvents() : Event<pT>("INTRINSIC_EVENT_DUMP") {}

        void execute()
        {
            dumpEvents();
        }

    } _stdout;

    class _dumpEventsToFile : public Event<pT>
    {
    public:

        _dumpEventsToFile() : Event<pT>("INTRINSIC_EVENT_FILEDUMP") {}

        void initialize()
        {
            Event<pT>::initializeEventMatrix();
        }

        void execute()
        {
            dumpEventsToFile();
        }

    } _fileio;



};
}
