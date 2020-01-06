#ifndef _CARRIER_ENV_HH_
#define _CARRIER_ENV_HH_

#include <mutex>
#include "DataFrame.hh"

namespace src
{

using Cid_t = int64_t;
class CarrierEnv;

class CarrierEnv
{
    private:
        Cid_t current_cell;
        std::vector<DataFrame> df;
        DataFrame prediction;

    private:
        std::mutex pred_lock;   /* lock for read/write predictions */

    public:
        CarrierEnv() = default;

        /**
         * initialize with several files
         */
        CarrierEnv(const std::vector<std::string> &files);

        /**
         * method updateCell. 
         * update the current cell.
         */
        void updateCell(Cid_t new_cell);

        /**
         * method updateLocation. 
         * update the location, which will trigger a prediction.
         * It will use the current location and current cell to predict
         */
        void updateLocation(double lng, double lat, double time);    // this will take a little bit long time
        
        /**
         * method getPrediction
         * get the current prediction
         */
        DataFrame getPrediction();
};

} // namespace src

#endif
