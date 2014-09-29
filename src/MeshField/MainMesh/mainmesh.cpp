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

}

template<typename pT>
void MainMesh<pT>::onConstruct()
{
    m_doOutput = false;

    m_storeEvents = false;

    m_storeEventsToFile = false;

    m_reportProgress = false;

    setOutputPath("/tmp/");

    BADAss(m_currentParticles, !=, NULL);

    for (uint i = 0; i < m_currentParticles->count(); ++i)
    {
        this->m_atoms.push_back(i);
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

    for (MeshField<pT> *subField : this->m_subFields)
    {
        subField->resetSubFields();
    }

    for (uint i = 0; i < this->m_particles->count(); ++i)
    {

        for (MeshField<pT> *subField : this->m_subFields)
        {
            (void)subField->checkSubFields(i);
        }

    }

}

template<typename pT>
void MainMesh<pT>::_finalize()
{
    for (Event<pT> *intrinsicEvent : m_intrinsicEvents)
    {
        delete intrinsicEvent;
    }

    m_intrinsicEvents.clear();


    for (LoopChunk *lc : m_allLoopChunks)
    {
        delete lc;
    }

    m_allLoopChunks.clear();

    m_allEvents.clear();


    m_storageEnabledEvents.clear();

    if (m_eventStorageFile.is_open())
    {
        m_eventStorageFile.close();
    }

}

template<typename pT>
void MainMesh<pT>::dumpLoopChunkInfo()
{

    using namespace std;

    for (LoopChunk * loopChunk : m_allLoopChunks) {

        cout << "Loopchunk interval: [" << loopChunk->m_start << " " << loopChunk->m_end << "]" << endl;
        cout << "has " << loopChunk->m_executeEvents.size() << " events: " << endl;
        for (Event<pT>* event : loopChunk->m_executeEvents) {
            cout << "  " << setw(2) << right << event->priority() << "  "
                 << setw(30) << left << event->type()
                 << "["
                 << setw(5) << event->onsetTime() << " "
                 << setw(5) << event->offsetTime()
                 << "]"
                 << endl;
        }

    }
}

template<typename pT>
void MainMesh<pT>::_streamValueToFile(const double value)
{
    BADAssBool(m_eventStorageFile.is_open(), "event file is not open but asked to write.");

    m_eventStorageFile.write(reinterpret_cast<const char*>(&value), sizeof(double));
}

template<typename pT>
void MainMesh<pT>::_storeEventValues(const uint index)
{

    for (uint i = 0; i < numberOfStoredEvents(); ++i)
    {
        const double &value = m_storageEnabledEvents.at(i)->value();

        if (m_storeEvents)
        {
            m_storedEventValues(index, i) = value;
        }

        if (m_storeEventsToFile)
        {
            this->_streamValueToFile(value);
        }

    }

}


template<typename pT>
void MainMesh<pT>::_initializeEventStorage(const uint size)
{

    for (Event<pT> *event : m_storageEnabledEvents)
    {
        m_storedEventTypes.push_back(event->type() + ("@" + event->meshField().description()));
    }

    if (m_storeEvents)
    {
        m_storedEventValues.zeros(size, numberOfStoredEvents());
    }

    if (m_storeEventsToFile)
    {
        m_eventStorageFile.open(m_outputPath + m_filename, std::ios::binary);

        uint nCols = m_storageEnabledEvents.size();

        m_eventStorageFile.write(reinterpret_cast<const char*>(&size), sizeof(uint));
        m_eventStorageFile.write(reinterpret_cast<const char*>(&nCols), sizeof(uint));

    }
}

template<typename pT>
void MainMesh<pT>::eventLoop(const uint nCycles)
{
    uint* loopCycle = new uint(0);

    _addIntrinsicEvents();

    this->_prepareEvents(nCycles, loopCycle);

    _sortEvents();

    _setupChunks();

    for (LoopChunk* loopChunk : m_allLoopChunks) {

        m_currentChunk = loopChunk;

        _initializeNewEvents();

        for (*loopCycle = m_currentChunk->m_start; *loopCycle <= m_currentChunk->m_end; ++(*loopCycle))
        {
            _updateContainments();

            _executeEvents();
        }

    }

    delete loopCycle;

    _finalize();

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
    m_allEvents.push_back(&event);

    if (event.storeValue())
    {
        m_storageEnabledEvents.push_back(&event);
    }
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

    if (m_storeEvents)
    {
        _dumpEventsToFile<pT> *_fileio = new _dumpEventsToFile<pT>(this);
        _fileio->setManualPriority();
        this->_addIntrinsicEvent(_fileio);
    }

}

template<typename pT>
void MainMesh<pT>::_sortEvents()
{
    std::sort(m_allEvents.begin(),
              m_allEvents.end(),
              [] (const Event<pT> *e1, const Event<pT> *e2) {return e1->priority() < e2->priority();});

    for (Event<pT> *event : m_allEvents)
    {
        for (const auto &dependency_pair: event->dependencies())
        {
            const Event<pT> *dependency = dependency_pair.second;

            if (dependency->priority() > event->priority())
            {
                std::stringstream s;
                s << "Dependency error: "
                  << event->description()
                  << " (" << event->priority() << ")"
                  << " depends on "
                  << dependency->description()
                  << " (" << dependency->priority() << ")";

                throw std::logic_error(s.str());
            }
        }
    }
}


template<typename pT>
void MainMesh<pT>::_initializeNewEvents()
{
    for (Event<pT>* event : m_currentChunk->m_executeEvents) {

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

    uvec onsetTimes(m_allEvents.size());
    uvec offsetTimes(m_allEvents.size());

    uint k = 0;
    for (Event<pT>* event : m_allEvents)
    {
        onsetTimes(k) = event->onsetTime();
        offsetTimes(k) = event->offsetTime();
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
                m_allLoopChunks.push_back(new LoopChunk(start, offsetTime));
                start = offsetTime + 1;
                //                if (debug) cout << "next interval starting at " << start << endl;

                jStart = j+1;

            }

            else
            {
                break;
            }
        }

        if (start <= end && (i != onsetTimes.n_elem - 1))
        {
            //            if (debug) cout << "adding remaining interval " << start << " - " << end << endl;
            m_allLoopChunks.push_back(new LoopChunk(start, end));
            start = end + 1;
        }
        //        if (debug) cout << "------------------------\n";

    }



    for (Event<pT>* event : m_allEvents) {
        for (LoopChunk* loopChunk : m_allLoopChunks) {
            if (event->onsetTime() <= loopChunk->m_start && event->offsetTime() >= loopChunk->m_end) {

                if (event->_hasExecuteImpl()) {
                    loopChunk->m_executeEvents.push_back(event);
                }

                if (event->_hasResetImpl()) {
                    loopChunk->m_resetEvents.push_back(event);
                }

            }
        }
    }

#ifndef NDEBUG
    dumpLoopChunkInfo();
#endif

}


template<typename pT>
void MainMesh<pT>::_executeEvents()
{

    for (Event<pT> * event : m_currentChunk->m_executeEvents)
    {
        event->execute();
    }

    for (Event<pT> * event : m_currentChunk->m_resetEvents)
    {
        event->reset();
    }

    for (Event<pT> * event : m_currentChunk->m_resetEvents)
    {
        event->_iterateCycle();
    }

}

template<typename pT>
void MainMesh<pT>::dumpEvents() const
{

    bool endline = false;
    for (Event<pT>* event : m_currentChunk->m_executeEvents)
    {
        if (event->hasOutput())
        {
            cout << event->dumpString() << endl;
            endline = true;
        }
    }

    if (endline)
    {
        cout << endl;
    }

}

template<typename pT>
PositionHandler<pT> *MainMesh<pT>::m_currentParticles = NULL;
