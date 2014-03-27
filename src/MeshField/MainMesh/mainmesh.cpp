
#include "mainmesh.h"

#include <assert.h>

#include "../../Event/event.h"
#include "../../Event/intrinsicevents.h"
#include "../../Ensemble/ensemble.h"

#include <iomanip>

using namespace ignis;


MainMesh::MainMesh(const mat &topology, Ensemble & ensemble):
    MeshField(topology, ensemble, "MainMesh"),
    m_silent(false),
    m_doFileIO(true)
{

    setOutputPath("/tmp/");

    for (uint i = 0; i < IGNIS_N; ++i) {
        atoms.push_back(i);
    }

}


void MainMesh::updateContainments()
{

    for (MeshField* subField : subFields){
        subField->resetSubFields();
    }

    for (uint i = 0; i < IGNIS_N; ++i) {

        for (MeshField* subField : subFields){
            (void)subField->checkSubFields(i);
        }

    }

}

void MainMesh::dumpLoopChunkInfo()
{

    using namespace std;

    for (LoopChunk * loopChunk : allLoopChunks) {

        cout << "Loopchunk interval: [" << loopChunk->start << " " << loopChunk->end << "]" << endl;
        cout << "has " << loopChunk->executeEvents.size() << " events: " << endl;
        for (Event* event : loopChunk->executeEvents) {
            cout << "  " << setw(2) << right << event->getPriority() << "  "
                 << setw(30) << left << event->getType()
                 << "["
                 << setw(5) << event->getOnsetTime() << " "
                 << setw(5) << event->getOffsetTime()
                 << "]"
                 << endl;
        }

    }
}

void MainMesh::dumpEventsToFile() const
{

    for (Event* event: currentChunk->executeEvents) {
        event->storeEvent();
    }

    Event::saveEventMatrix(outputPath);
}


void MainMesh::eventLoop(uint N)
{

    addIntrinsicEvents();

    uint* loopCycle = new uint(0);

    Event::setNumberOfCycles(N);
    Event::setLoopCyclePtr(loopCycle);

    prepareEvents();

    sortEvents();

    setupChunks();

    for (LoopChunk* loopChunk : allLoopChunks) {

        currentChunk = loopChunk;

        initializeNewEvents();

        for (*loopCycle = currentChunk->start; *loopCycle <= currentChunk->end; ++(*loopCycle)) {

            updateContainments();

            executeEvents();

        }

    }

}

void MainMesh::setOutputPath(std::string path)
{
    if (strcmp(&path.back(), "/") != 0){
        path = path + "/";
    }

    outputPath = path + "mdEventsOut.arma";
}

void MainMesh::sendToTop(Event &event)
{
    allEvents.push_back(&event);
}

void MainMesh::addIntrinsicEvents()
{

    if (!m_silent)
    {
        Event *_stdout = new _dumpEvents(this);
        _stdout->setManualPriority();
        addEvent(*_stdout);
    }

    if (!m_doFileIO)
    {
        Event *_fileio = new _dumpEventsToFile(this);
        _fileio->setManualPriority();
        addEvent(*_fileio);
    }

}

void MainMesh::sortEvents()
{
    std::sort(allEvents.begin(),
              allEvents.end(),
              [] (const Event *e1, const Event *e2) {return e1->getPriority() < e2->getPriority();});
}

void MainMesh::initializeNewEvents()
{
    for (Event* event : currentChunk->executeEvents) {
        event->_initEvent();
    }
}

void MainMesh::setupChunks()
{

    uvec onsetTimes(Event::getTotalCounter());
    uvec offsetTimes(Event::getTotalCounter());

    assert(Event::getTotalCounter() == allEvents.size() && "Mismatch in event sizes...");

    uint k = 0;
    for (Event* event : allEvents) {
        onsetTimes(k) = event->getOnsetTime();
        offsetTimes(k) = event->getOffsetTime();
        k++;
    }


    onsetTimes = unique(onsetTimes);
    offsetTimes = unique(offsetTimes);

    uint start = onsetTimes(0);
    uint end;

    uint jStart = 0;
    uint offsetTime;

    //    bool debug = false;

    for (uint i = 0; i < onsetTimes.n_elem; ++i) {

        if (i != onsetTimes.n_elem - 1)
        {
            end = onsetTimes(i+1) - 1;
        }

        else
        {
            end = offsetTimes(offsetTimes.n_elem - 1) + 1;
        }

        //        if (debug) cout << "start at " << start << endl;
        //        if (debug) cout << "end " << end << endl;

        for (uint j = jStart; j < offsetTimes.n_elem; ++j) {

            offsetTime = offsetTimes(j);

            //            if (debug) cout << "testing offsettime " << offsetTime << endl;

            if (offsetTime <= end)
            {
                //                if (debug) cout << "adding interval " << start << " - " << offsetTime << endl;
                allLoopChunks.push_back(new LoopChunk(start, offsetTime));
                start = offsetTime + 1;
                //                if (debug) cout << "next interval starting at " << start << endl;

                jStart = j+1;

            }

            else
            {
                break;
            }
        }

        if (start < end)
        {
            //            if (debug) cout << "adding remaining interval " << start << " - " << end << endl;
            allLoopChunks.push_back(new LoopChunk(start, end));
            start = end + 1;
        }
        //        if (debug) cout << "------------------------\n";

    }



    for (Event* event : allEvents) {
        for (LoopChunk* loopChunk : allLoopChunks) {
            if (event->getOnsetTime() <= loopChunk->start && event->getOffsetTime() >= loopChunk->end) {

                if (event->_hasExecuteImpl()) {
                    loopChunk->executeEvents.push_back(event);
                }

                if (event->_hasResetImpl()) {
                    loopChunk->resetEvents.push_back(event);
                }

            }
        }
    }

    dumpLoopChunkInfo();

}

void MainMesh::executeEvents()
{

    for (Event * event : currentChunk->executeEvents) {
        event->executeEvent();
    }

    for (Event * event : currentChunk->executeEvents) {
        event->reset();
    }

}

void MainMesh::dumpEvents() const
{

    for (Event* event : currentChunk->executeEvents)
    {
        if (event->notSilent())
        {
            cout << event->dumpString() << endl;
        }
    }

    cout << endl;
}



