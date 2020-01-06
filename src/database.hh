#ifndef _DATABASE_HH_
#define _DATABASE_HH_

#include <map>
#include "../common/common.hh"

namespace db
{

struct Prediction;  // TODO: define the data-structure! 

/* initialization */
void Initialize(const std::map<Carrier_t, std::string> &dir_files); // in DatabaseContext.cc

/* unblocking interface for update states */
void UpdateGPS(double lng, double lat, double time);    // in DatabaseContext.cc
void UpdateCellID(Cid_t cell_id, Carrier_t carrier);    // in DatabaseContext.cc

/* unblocking interface for get value */
// TODO: define a Prediction for it!
Prediction GetPrediction(Carrier_t crr);

/* communication interface */
// TODO: communication scheme when using multi-process DB

} // namespace db

#endif
