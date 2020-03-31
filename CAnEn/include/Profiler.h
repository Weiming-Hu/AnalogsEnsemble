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

// Include openmp header files if we are using it
#if defined(_OPENMP)
#include <omp.h>
#endif

/*
 * Based on different operating systems, we include different header files
 * to read process memory usage. If the operating system is not supported,
 * memory usage information will be disable by defining _UNKNOWN_OS_
 * 
 * This approach is adopted from David Robert Nadeau. References below:
 * - 
 * 
 */
#if defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>

#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
#include <fcntl.h>
#include <procfs.h>

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include <stdio.h>

#endif

#else
#define _UNKNOWN_OS_
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
    
#ifndef _UNKNOWN_OS_
    std::vector<std::size_t> peak_memory_;
    std::vector<std::size_t> current_memory_;
    
    std::size_t getPeakRSS_();
    std::size_t getCurrentRSS_();

    int max_name_width_() const;
#endif
    
};

#endif /* PROFILER_H */

