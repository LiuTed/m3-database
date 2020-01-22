#ifndef _DATABASECONTEXT_HH_
#define _DATABASECONTEXT_HH_

#include <vector>
#include <map>
#include <string>
#include "Queue.hh"
#include "CarrierEnv.hh"

/**
 * The real running database.
 * Only one "DatabaseContext" object can exist in the whole program.
 * There is 2 threads in the object.
 *  1. thread for uploading requests and getting results
 *  2. the main thread
 */

namespace db
{

class DatabaseContext;  /* real running database */

} // namespace db

namespace src
{
class UpdateRequest;
} // namespace src

namespace db
{

/**
 * class DatabaseContext
 * The class for real running database
 * To use it: 
 *  call static method GetDatabaseContext() to get the handle
 *  call static method DeleteDatabaseContext() to release the memory
 *  call handle->initialize(...) to initialize a carrier
 *  call handle->start() to start main loop
 */
class DatabaseContext
{

    private:    /* private fields */
        std::map<Carrier_t, src::CarrierEnv> carriers;
        src::ConsumerProducerQueue<src::UpdateRequest> requestQueue;

        /* running control */
        std::mutex run_lock;
        std::mutex wait_mutex;  /* control the wait_cond, also secure the access to requestQueue */
        std::condition_variable wait_cond;

        int finished_jobid = 0;
        std::mutex jobid_mutex;

    private:    /* private static fields */
        static DatabaseContext *pDBContext;


    private:    /* private methods */
        DatabaseContext() {this->start();};
        ~DatabaseContext();
        void processGPS(const src::UpdateRequest &r);
        void processCell(const src::UpdateRequest &r);
        void mainLoop();    /* processing event queue */
        void start();   /* run the main thread */

    public:     /* public methods */
        void initialize(Carrier_t carrier, 
                    const std::vector<std::string> &datafiles,
                    const std::vector<std::string> &hofiles,
                    const std::vector<std::string> &bdfiles);

        void addRequest(src::UpdateRequest &req);  /* non-blocking */
        src::DataFrame getPrediction(Carrier_t cr); /* non-blocking */
        void wait();    /* blocking until queue is empty */
        int getFinishedJobid(); /* get finished_jobid */

    public:     /* public static methods */
        static DatabaseContext   *GetDatabaseContext();
        static void               DeleteDatabaseContext();
        static src::UpdateRequest forgeGPSRequest(double lng, double lat, double time);
        static src::UpdateRequest forgeCellRequest(src::Cid_t cid, Carrier_t carrier);
};

} // namespace db

namespace src
{

/**
 * for gps request:
 *  type = GPS
 *  values = {lng, lat, time}, size = 3
 * for cell request:
 *  typ = CELL
 *  values = {cid, carrier}
 */
class UpdateRequest
{
    public:
        enum RequestType {GPS, CELL, STOP};
        int jobid;
    public:
        RequestType type;
        std::vector<double> values; 
};

} // namespace src

#endif
