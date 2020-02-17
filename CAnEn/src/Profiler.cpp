/* 
 * File:   Profiler.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 12, 2020, 11:24 AM
 */

#include <iomanip>
#include <stdexcept>

#include "Profiler.h"
#include "boost/date_time.hpp"

using namespace std;
using namespace boost::posix_time;

static const string _SESSION_START = "start";

Profiler::Profiler(size_t num_sessions) {
    session_names_.reserve(num_sessions);
    ptimes_.reserve(num_sessions);

#if defined(_OPENMP)
    wtimes_.reserve(num_sessions);
#endif
}

Profiler::~Profiler() {
}

void
Profiler::start() {
    log_time_session(_SESSION_START);
    return;
}

void
Profiler::log_time_session(const std::string& session_name) {
    session_names_.push_back(session_name);
    ptimes_.push_back(clock());

#if defined(_OPENMP)
    wtimes_.push_back(omp_get_wtime());
#endif
}

void
Profiler::summary(std::ostream& os) const {
    
    // Sanity check
    size_t num_sessions = session_names_.size();
    if (num_sessions <= 1) throw runtime_error("No sessions to report. Did you call start/log_time_session/end?");
    if (session_names_.front() != _SESSION_START) throw runtime_error("You didn't start the profiler by calling start");
    
    // Define time variables
    float full_pduration = (ptimes_.back() - ptimes_.front()) / (float) CLOCKS_PER_SEC;
    
#if defined(_OPENMP)
    double full_wduration = wtimes_.back() - wtimes_.front();
#endif

    // Start printing
    os << setprecision(3) << fixed;
    os << "****************     Profiler Summary    *****************" << endl;
    
    for (size_t session_i = 1; session_i < num_sessions; ++session_i) {
        
        // Calculate processor time consumption
        clock_t ptime = (ptimes_[session_i] - ptimes_[session_i - 1]) / CLOCKS_PER_SEC;
        time_duration ptime_duration = seconds(ptime);
        
#if defined(_OPENMP)
        // Calculate wall time consumption
        long wtime = wtimes_[session_i] - wtimes_[session_i - 1];
        time_duration wtime_duration = seconds(wtime);
#endif
        
        os << "Session " << session_names_[session_i] <<
                ":\t processor time (" << to_simple_string(ptime_duration)
                << ", " << ptime/full_pduration * 100 << "%)"
#if defined(_OPENMP)
                << "\t wall time (" << to_simple_string(wtime_duration)
                << ", " << wtime/full_wduration * 100 << "%)"
#endif
                << endl;
                
    }
    os << "**************** End of Profiler Summary *****************" << endl;

    
}
