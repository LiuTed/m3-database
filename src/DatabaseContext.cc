#include <thread>
#include "database.hh"
#include "DatabaseContext.hh"

namespace db
{
/* ================== DatabaseContext ================== */
DatabaseContext *DatabaseContext::pDBContext = NULL;

void DatabaseContext::mainLoop()
{
    std::unique_lock<std::mutex> lock(this->run_lock); // lock the mutex
    bool running = true;
    while(running)
    {
        auto request = requestQueue.top();
        LOG_DEBUG("DatabaseContext::mainLoop: got request! Type is ", request.type);
        switch(request.type)
        {
            case src::UpdateRequest::GPS:
                this->processGPS(request);
                break;
            case src::UpdateRequest::CELL:
                this->processCell(request);
                break;
            case src::UpdateRequest::STOP:
                running = false;
                break;
        }
        requestQueue.pop();

        /* finished execution, modify finished_jobid */
        {
            std::unique_lock<std::mutex> lk(this->jobid_mutex);
            this->finished_jobid = request.jobid;
        }

        /* check if it's empty, and notify wait condition */
        {
            std::unique_lock<std::mutex> lk(this->wait_mutex);
            if(requestQueue.isEmpty())
                wait_cond.notify_all();
        }
    }
}

void DatabaseContext::processGPS(const src::UpdateRequest &r)
{
    auto lng = r.values[0],
         lat = r.values[1],
         time = r.values[2];
    for(auto & ent : this->carriers)
        ent.second.updateLocation(lng, lat, time);
}

void DatabaseContext::processCell(const src::UpdateRequest &r)
{
    src::Cid_t cell = r.values[0];
    Carrier_t crr = (Carrier_t)((int)r.values[1]);
    if(this->carriers.count(crr))
        this->carriers[crr].updateCell(cell);
}

void DatabaseContext::start()
{
    //this->isRunning = true;
    std::thread t([this](){this->mainLoop();});
    t.detach(); /* add the new thread */
}

void DatabaseContext::initialize(Carrier_t crr, 
        const std::vector<std::string> &datafiles,
        const std::vector<std::string> &hofiles,
        const std::vector<std::string> &bdfiles)
{
    this->carriers.emplace(std::piecewise_construct,
            std::forward_as_tuple(crr),
            std::forward_as_tuple(datafiles, hofiles, bdfiles));
}


void DatabaseContext::addRequest(src::UpdateRequest &req)
{
    static int total_jobs = 0;
    std::unique_lock<std::mutex> lock(this->wait_mutex);
    /* increase the jobid and insert the request */
    total_jobs += 1;
    req.jobid = total_jobs;
    this->requestQueue.add(req);
    //LOG_DEBUG("DatabaseContext::addRequest: new request added!");
}

src::DataFrame DatabaseContext::getPrediction(Carrier_t cr)
{
    if(this->carriers.count(cr))
        return this->carriers[cr].getPrediction();
    else throw std::runtime_error("DatabaseContext::getPrediction: unrecognized carrier or uninitialized DatabaseContext");
}

void DatabaseContext::wait()
{
    //while(!this->requestQueue.isEmpty())
    //  this->wait_cond.wait(...);
    std::unique_lock<std::mutex> wait_lock(wait_mutex);
    this->wait_cond.wait(wait_lock, [this](){return this->requestQueue.isEmpty();});
}

DatabaseContext *DatabaseContext::GetDatabaseContext()
{
    if(DatabaseContext::pDBContext == NULL)
        DatabaseContext::pDBContext = new DatabaseContext();
    return DatabaseContext::pDBContext;
}

void DatabaseContext::DeleteDatabaseContext()
{
    DatabaseContext *&pdbc = DatabaseContext::pDBContext;
    if(pdbc)
    {
        delete pdbc;
        pdbc = NULL;
    }
}

src::UpdateRequest DatabaseContext::forgeGPSRequest(double lng, double lat, double time)
{
    src::UpdateRequest ret;
    ret.type = src::UpdateRequest::GPS;
    ret.values.push_back(lng);
    ret.values.push_back(lat);
    ret.values.push_back(time);
    return ret;
}

src::UpdateRequest DatabaseContext::forgeCellRequest(src::Cid_t cid, Carrier_t carrier)
{
    src::UpdateRequest ret;
    ret.type = src::UpdateRequest::CELL;
    ret.values.push_back(cid);
    ret.values.push_back((int)carrier);
    return ret;
}

int DatabaseContext::getFinishedJobid()
{
    std::unique_lock<std::mutex> lk(this->jobid_mutex);
    return this->finished_jobid;
}

DatabaseContext::~DatabaseContext()
{
    //this->isRunning = false;
    LOG_DEBUG("Ready to destruct!");
    src::UpdateRequest stop_request;
    stop_request.type = src::UpdateRequest::STOP;
    this->addRequest(stop_request);
    {
        std::unique_lock<std::mutex> lock(this->run_lock);  // wait until mainLoop ends
    }
}

/* ============== interface in database.hh ==================== */
void Initialize(const std::map<Carrier_t, std::string> &dir_files)
{
    auto pdbc = DatabaseContext::GetDatabaseContext();  // create if not exist
    for(auto &ent : dir_files)
    {
        auto carrier = ent.first;
        auto dirfile = ent.second;
        common::input_helper helper(dirfile);
        std::vector<std::string> datafiles, hofiles, bdfiles;
        datafiles.clear();
        hofiles.clear();
        bdfiles.clear();
        while(helper.hasNext())
        {
            auto vec = helper.next();
            if(vec.empty()) continue;
            if(vec[0] == "F") break;
            datafiles.push_back(vec[0]);
        }

        /* hofiles */
        while(helper.hasNext())
        {
            auto vec = helper.next();
            if(vec.empty()) continue;
            if(vec[0] == "F") break;
            hofiles.push_back(vec[0]);
        }

        /* dbfiles */
        while(helper.hasNext())
        {
            auto vec = helper.next();
            if(vec.empty()) continue;
            if(vec[0] == "F") break;
            bdfiles.push_back(vec[0]);
        }

        pdbc->initialize(carrier, datafiles, hofiles, bdfiles);
    }
}


int UpdateGPS(double lng, double lat, double time)
{
    auto req = DatabaseContext::forgeGPSRequest(lng, lat, time);
    DatabaseContext::GetDatabaseContext()->addRequest(req);
    return req.jobid;
}

int UpdateCellID(src::Cid_t cell, Carrier_t carrier)
{
    auto req = DatabaseContext::forgeCellRequest(cell, carrier);
    DatabaseContext::GetDatabaseContext()->addRequest(req);
    return req.jobid;
}

Prediction GetPrediction(Carrier_t crr)
{
    return DatabaseContext::GetDatabaseContext()->getPrediction(crr);
}

int GetFinishedJobid()
{
    return DatabaseContext::GetDatabaseContext()->getFinishedJobid();
}

}   // namespace db
