/* 
 * File:   Profiler.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 12, 2020, 11:24 AM
 */

#include <iomanip>
#include <stdexcept>
#include <iomanip>

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

#ifndef _UNKNOWN_OS_
    peak_memory_.reserve(num_sessions);
    current_memory_.reserve(num_sessions);
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

#ifndef _UNKNOWN_OS_
    peak_memory_.push_back(getPeakRSS_());
    current_memory_.push_back(getCurrentRSS_());
#endif
}

void
Profiler::append_sessions(const Profiler & new_sessions) {
    size_t num_sub_sessions = new_sessions.session_names_.size();

    // If the input profiler does not log any sessions. By default, there is
    // a start session in the profiler, so the minimum should be 2.
    //
    if (num_sub_sessions < 2) return;

    for (size_t i = 1; i < num_sub_sessions; ++i) {
        session_names_.push_back(string("  -- ") + new_sessions.session_names_[i]);
        ptimes_.push_back(new_sessions.ptimes_[i]);
        
#if defined(_OPENMP)
        wtimes_.push_back(new_sessions.wtimes_[i]);
#endif

#ifndef _UNKNOWN_OS_
        peak_memory_.push_back(new_sessions.peak_memory_[i]);
        current_memory_.push_back(new_sessions.current_memory_[i]);
#endif
    }

    return;
}

void
Profiler::operator+=(const Profiler & rhs) {
    append_sessions(rhs);
    return;
}

void
Profiler::summary(std::ostream& os) const {

    // Sanity check
    size_t num_sessions = session_names_.size();
    if (num_sessions <= 1) throw runtime_error("No sessions to report. Did you call start/log_time_session/end?");
    if (session_names_.front() != _SESSION_START) throw runtime_error("You didn't start the profiler by calling start");
    int max_width = max_name_width_();

    // Define time variables
    long full_pduration = (ptimes_.back() - ptimes_.front()) / CLOCKS_PER_SEC;

#if defined(_OPENMP)
    long full_wduration = wtimes_.back() - wtimes_.front();
#endif

    // Start printing
    os << setprecision(3) << fixed;
    os << "****************     Profiler Summary    *****************" << endl;

    // Print the total session time
    time_duration ptime_duration = seconds(full_pduration);
#if defined(_OPENMP)
    time_duration wtime_duration = seconds(full_wduration);
#endif

    os << setw(max_width) << "Total" << ": processor time (" << to_simple_string(ptime_duration) << ", " << setw(6) << "100" << "%)"
#if defined(_OPENMP)
            << "\t wall time (" << to_simple_string(wtime_duration) << ", " << setw(6) << "100" << "%)"
#endif
            << endl;

    for (size_t session_i = 1; session_i < num_sessions; ++session_i) {

        // Calculate processor time consumption
        clock_t ptime = (ptimes_[session_i] - ptimes_[session_i - 1]) / CLOCKS_PER_SEC;
        ptime_duration = seconds(ptime);

#if defined(_OPENMP)
        // Calculate wall time consumption
        long wtime = wtimes_[session_i] - wtimes_[session_i - 1];
        wtime_duration = seconds(wtime);
#endif

        os << setw(max_width) << session_names_[session_i] <<
                ": processor time (" << to_simple_string(ptime_duration)
                << ", " << setw(6) << ptime / (double) full_pduration * 100 << "%)"
#if defined(_OPENMP)
                << "\t wall time (" << to_simple_string(wtime_duration)
                << ", " << setw(6) << wtime / (double) full_wduration * 100 << "%)"
#endif

#ifndef _UNKNOWN_OS_
                << "\t current memory (" << current_memory_[session_i] 
                << " bytes)\t peak memory (" << peak_memory_[session_i]
                << " bytes)"
#endif
                << endl;

    }
    os << "**************** End of Profiler Summary *****************" << endl;

}


#ifndef _UNKNOWN_OS_

std::size_t Profiler::getCurrentRSS_() {

#if defined(__APPLE__) && defined(__MACH__)
    // OSX
    struct mach_task_basic_info info;

    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
            (task_info_t) & info, &infoCount) != KERN_SUCCESS)
        return (size_t) 0L; /* Can't access? */

    return (size_t) info.resident_size;

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    // Linux
    long rss = 0L;
    FILE* fp = NULL;

    if ((fp = fopen("/proc/self/statm", "r")) == NULL) return (size_t) 0L; /* Can't open? */

    if (fscanf(fp, "%*s%ld", &rss) != 1) {
        fclose(fp);
        return (size_t) 0L; /* Can't read? */
    }

    fclose(fp);

    return (size_t) rss * (size_t) sysconf(_SC_PAGESIZE);

#else
    return (size_t) 0L; /* Unsupported. */
#endif
}

std::size_t Profiler::getPeakRSS_() {

#if (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
    // AIX and Solaris
    struct psinfo psinfo;
    int fd = -1;

    if ((fd = open("/proc/self/psinfo", O_RDONLY)) == -1) return (size_t) 0L; /* Can't open? */

    if (read(fd, &psinfo, sizeof (psinfo)) != sizeof (psinfo)) {
        close(fd);
        return (size_t) 0L; /* Can't read? */
    }

    close(fd);

    return (size_t) (psinfo.pr_rssize * 1024L);

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
    // BSD, Linux, and OSX
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);

#if defined(__APPLE__) && defined(__MACH__)
    return (size_t) rusage.ru_maxrss;

#else
    return (size_t) (rusage.ru_maxrss * 1024L);
#endif

#else
    // Unknown OS
    return (size_t) 0L; /* Unsupported. */
#endif
}

#endif

int
Profiler::max_name_width_() const {
    if (session_names_.size() == 0) throw runtime_error("No session names added. Please add session names with log_time_session");

    // Initialize the width to the size of "Total"
    int max_name_width = 5;
    for (const auto & name : session_names_) {
        if (name.size() > max_name_width) max_name_width = name.size();
    }

    return max_name_width;
}
