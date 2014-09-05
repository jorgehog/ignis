#include "mainmesh.h"

#include "intrinsicevents.h"

#include <iomanip>


using namespace ignis;


template<typename pT>
MainMesh<pT>::MainMesh() :
    MeshField<pT>(std::string("MainMesh"))
{
    onConstruct();
}

template<typename pT>
MainMesh<pT>::MainMesh(const Mat<pT> &topology) :
    MeshField<pT>(topology, "MainMesh")
{
    onConstruct();
}

template<typename pT>
MainMesh<pT>::MainMesh(const std::initializer_list<pT> topology) :
    MeshField<pT>(topology, "MainMesh")
{
    onConstruct();
}

template<typename pT>
MainMesh<pT>::~MainMesh()
{

    for (Event<pT> *intrinsicEvent : m_intrinsicEvents)
    {
        delete intrinsicEvent;
    }

    for (LoopChunk *lc : allLoopChunks)
    {
        delete lc;
    }

}

template<typename pT>
void MainMesh<pT>::onConstruct()
{
    setOutputPath("/tmp/");

    assert(m_currentParticles != NULL);

    for (uint i = 0; i < m_currentParticles->count(); ++i)
    {
        this->atoms.push_back(i);
    }
}

template<typename pT>
uint MainMesh<pT>::getPopulation() const
{
    return MeshField<pT>::totalNumberOfParticles();
}

template<typename pT>
void MainMesh<pT>::_updateContainments()
{

    for (MeshField<pT> *subField : this->subFields)
    {
        subField->resetSubFields();
    }

    for (uint i = 0; i < this->m_particles->count(); ++i)
    {

        for (MeshField<pT> *subField : this->subFields)
        {
            (void)subField->checkSubFields(i);
        }

    }

}

template<typename pT>
void MainMesh<pT>::_dumpLoopChunkInfo()
{

    using namespace std;

    for (LoopChunk * loopChunk : allLoopChunks) {

        cout << "Loopchunk interval: [" << loopChunk->start << " " << loopChunk->end << "]" << endl;
        cout << "has " << loopChunk->executeEvents.size() << " events: " << endl;
        for (Event<pT>* event : loopChunk->executeEvents) {
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

template<typename pT>
void MainMesh<pT>::storeEventValues() const
{

    for (Event<pT>* event: currentChunk->executeEvents)
    {
        event->storeEvent();
    }

}


template<typename pT>
void MainMesh<pT>::eventLoop()
{

    _addIntrinsicEvents();

    uint* loopCycle = new uint(0);

    Event<pT>::setNumberOfCycles(nCycles);

    Event<pT>::setLoopCyclePtr(loopCycle);

    this->prepareEvents();

    _sortEvents();

    _setupChunks();

    for (LoopChunk* loopChunk : allLoopChunks) {

        currentChunk = loopChunk;

        _initializeNewEvents();

        for (*loopCycle = currentChunk->start; *loopCycle <= currentChunk->end; ++(*loopCycle))
        {
            _updateContainments();

            _executeEvents();
        }

    }

    delete loopCycle;

}

template<typename pT>
void MainMesh<pT>::setOutputPath(std::string path)
{
    if (strcmp(&path.back(), "/") != 0){
        path = path + "/";
    }

    m_outputPath = path;
}

template<typename pT>
void MainMesh<pT>::_sendToTop(Event<pT> &event)
{
    allEvents.push_back(&event);
}

template<typename pT>
void MainMesh<pT>::_addIntrinsicEvents()
{

    if (m_reportProgress)
    {
        _reportProgress<pT> *_prog = new _reportProgress<pT>();
        _prog->setManualPriority();
        this->_addIntrinsicEvent(_prog);
    }

    if (m_doOutput)
    {
        _dumpEvents<pT> *_stdout = new _dumpEvents<pT>(this);
        _stdout->setManualPriority();
        this->_addIntrinsicEvent(_stdout);
    }

    if (m_doFileIO && Event<pT>::getCounter() != 0)
    {
        if (((m_saveFileSpacing%m_saveValuesSpacing) != 0) ||
                (m_saveFileSpacing < m_saveValuesSpacing))
        {
            cerr << "saving file every " << m_saveFileSpacing
                 << " mismatch with saving values every" << m_saveValuesSpacing << " cycle.";
            exit(1);
        }

        _dumpEventsToFile<pT> *_fileio = new _dumpEventsToFile<pT>(this);
        _fileio->setManualPriority();
        this->_addIntrinsicEvent(_fileio);
    }

}

template<typename pT>
void MainMesh<pT>::_sortEvents()
{
    std::sort(allEvents.begin(),
              allEvents.end(),
              [] (const Event<pT> *e1, const Event<pT> *e2) {return e1->getPriority() < e2->getPriority();});
}


template<typename pT>
void MainMesh<pT>::_initializeNewEvents()
{
    for (Event<pT>* event : currentChunk->executeEvents) {

        if (!event->initialized())
        {
            event->initialize();
            event->markAsInitialized();
        }
    }
}


template<typename pT>
void MainMesh<pT>::_setupChunks()
{

    uvec onsetTimes(Event<pT>::getTotalCounter());
    uvec offsetTimes(Event<pT>::getTotalCounter());

    //    assert(Event<pT>::getTotalCounter() == allEvents.size() && "Mismatch in event sizes...");

    uint k = 0;
    for (Event<pT>* event : allEvents) {
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



    for (Event<pT>* event : allEvents) {
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

    //    dumpLoopChunkInfo();


}


template<typename pT>
void MainMesh<pT>::_executeEvents()
{

    for (Event<pT> * event : currentChunk->executeEvents) {
        event->_executeEvent();
    }

    for (Event<pT> * event : currentChunk->executeEvents) {
        event->reset();
    }

}

template<typename pT>
void MainMesh<pT>::dumpEvents() const
{

    for (Event<pT>* event : currentChunk->executeEvents)
    {
        if (event->notSilent())
        {
            cout << event->dumpString() << endl;
        }
    }

    cout << endl;
}



template<typename pT>
uint MainMesh<pT>::nCycles = 0;

template<typename pT>
bool MainMesh<pT>::m_doOutput = true;

template<typename pT>
uint MainMesh<pT>::m_outputSpacing = 1;

template<typename pT>
bool MainMesh<pT>::m_doFileIO = true;

template<typename pT>
uint MainMesh<pT>::m_saveValuesSpacing = 1;

template<typename pT>
uint MainMesh<pT>::m_saveFileSpacing = 1000;

template<typename pT>
std::string MainMesh<pT>::m_filename = "ignisEventsOut.arma";

template<typename pT>
bool MainMesh<pT>::m_reportProgress = true;

template<typename pT>
PositionHandler<pT> *MainMesh<pT>::m_currentParticles = NULL;
