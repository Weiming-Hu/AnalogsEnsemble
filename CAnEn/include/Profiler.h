/* 
 * File:   Profiler.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 12, 2020, 11:24 AM
 */

#ifndef PROFILER_H
#define PROFILER_H

#include <vector>
#include <iostream>
#include <ctime>

#if defined(_OPENMP)
#include <omp.h>
#endif

class Profiler {
public:
    Profiler(std::size_t num_sessions = 10);
    virtual ~Profiler();
    
    void start();
    void log_time_session(const std::string & session_name);
    
    void summary(std::ostream &) const;
    
private:
    std::vector<std::string> session_names_;
    std::vector<clock_t> ptimes_;

#if defined(_OPENMP)
    std::vector<double> wtimes_;
#endif
    
};

#endif /* PROFILER_H */

