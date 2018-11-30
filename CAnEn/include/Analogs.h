/* 
 * File:   Analogs.h
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 5, 2018, 5:00 PM
 */

#ifndef ANALOGS_H
#define ANALOGS_H

#include "boost/multi_array.hpp"
#include "Forecasts.h"

/**
 * \class Analogs
 * 
 * \brief This is the class to store analogs. The default dimensions are:
 *           [test stations][test times][FLTs][members][3]
 * 
 * The fifth dimension stores in sequence the observed value, the search
 * time indicator, and the search station indicator.
 * 
 * Analogs data order is column-major.
 */
class Analogs : public boost::multi_array<double, 5> {
public:
    Analogs();
    Analogs(size_t num_stations, size_t num_times,
            size_t num_flts, size_t num_members);
    Analogs(const Forecasts & forecasts, size_t num_members);
    Analogs(const Analogs& orig) = delete;
    virtual ~Analogs();

    enum COL_TAG {
        UNKNOWN = -999,
        VALUE = 0,
        STATION = 1,
        TIME = 2
    };

    anenTime::FLTs getFLTs() const;
    anenSta::Stations getStations() const;
    anenTime::Times getTimes() const;

    void setFLTs(anenTime::FLTs flts);
    void setStations(anenSta::Stations stations);
    void setTimes(anenTime::Times times);

    void print(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &, const Analogs &);

    std::vector< std::vector<size_t> > search_stations;

private:
    anenSta::Stations stations_;
    anenTime::Times times_;
    anenTime::FLTs flts_;
};

#endif /* ANALOGS_H */

