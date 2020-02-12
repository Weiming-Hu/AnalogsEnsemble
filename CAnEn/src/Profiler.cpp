/* 
 * File:   Profiler.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 12, 2020, 11:24 AM
 */

#include <iomanip>
#include <stdexcept>

#include "Profiler.h"
#include <boost/date_time.hpp>

using namespace std;
using namespace boost::posix_time;

static const string _SESSION_START = "start";
static const string _SESSION_END = "end";

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
    ptimes_.push_back(clock_t());

#if defined(_OPENMP)
    wtimes_.push_back(omp_get_wtime());
#endif
}

void
Profiler::end() {
    log_time_session(_SESSION_END);
    return;
}

void
Profiler::summary(std::ostream& os) const {
    
    // Sanity check
    size_t num_sessions = session_names_.size();
    if (num_sessions <= 1) throw runtime_error("No sessions to report. Did you call start/log_time_session/end?");
    if (session_names_.front() != _SESSION_START) throw runtime_error("You didn't start the profiler by calling start");
    if (session_names_.back() != _SESSION_END) throw runtime_error("You didn't close the profiler by calling end");
    
    // Define time variables
    clock_t ptime;
    time_duration ptime_duration;
    float full_pduration = (ptimes_.back() - ptimes_.front()) / (float) CLOCKS_PER_SEC;
    
#if defined(_OPENMP)
    long wtime;
    time_duration wtime_duration;
    double full_wduration = wtimes_.back() - wtimes_.front();
#endif

    // Start printing
    os << setprecision(6) << fixed;
    os << "**************** Profiler Summary *****************" << endl;
    
    for (size_t session_i = 1; session_i < num_sessions - 1; ++session_i) {
        
        // Calculate processor time consumption
        ptime = (ptimes_[session_i] - ptimes_[session_i - 1]) / CLOCKS_PER_SEC;
        ptime_duration = seconds(ptime);
        
#if defined(_OPENMP)
        // Calculate wall time consumption
        wtime = wtimes_[session_i] - wtimes_[session_i - 1];
        wtime_duration = seconds(wtime);
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
    
}
