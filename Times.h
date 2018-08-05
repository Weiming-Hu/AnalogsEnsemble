/* 
 * File:   Time.h
 * Author: guido
 *
 * Created on April 18, 2018, 12:33 AM
 */

#ifndef TIME_H
#define TIME_H

#include <string>
#include <iostream>

#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/random_access_index.hpp>

/**
 * The anenTime name space is created for classes Times and FLTs.
 */
namespace anenTime {

    struct by_insert {
        /**
         * The tag for insertion sequence indexing
         */
    };

    struct by_value {
        /**
         * The tag for value-based indexing
         */
    };

    /**
     * Base class for Times
     */
    using multiIndexTimes = boost::multi_index_container<

            // This is the base class
            double,

            boost::multi_index::indexed_by<

            // Order by insertion sequence
            boost::multi_index::random_access<
            boost::multi_index::tag<by_insert> >,

            // Order by value
            boost::multi_index::hashed_unique<
            boost::multi_index::tag<by_value>,
            boost::multi_index::identity<std::size_t> > > >;

    /**
     * \class Times
     * 
     * \brief Times class is used to store time information for predictions. By 
     * default this is the number of seconds from the origin January 1st, 1970.
     * This can be customized by changing the default setting of origin and unit.
     * 
     * Times class supports the following features:
     * 1. Timestamps are unique in Times;
     * 2. Timestamps are kept in sequence of insertion, and have random access;
     * 3. Timestamps are accessible via values.
     */
    class Times : public multiIndexTimes {
    public:
        Times();
        Times(std::string unit);
        Times(std::string unit, std::string origin);

        virtual ~Times();

        bool getTimeIndex(double timestamp, size_t & i_time) const;

        void print(std::ostream & os) const;
        friend std::ostream& operator<<(std::ostream& os, Times const & obj);

    protected:
        std::string unit_ = "seconds";
        std::string origin_;
    };

    /**
     * \class FLTs
     * 
     * \brief FLTs class is used to store time information for prediction forecast
     * lead times (FLTs). If a temperature forecast on January 1st, 2028 contains
     * predictions at 00h, 06h, 12h, 18h on that day, this forecast has 4 FLTs,
     * which are 0, 6, 12, 18.
     * 
     * FLTs class supports the following features:
     * 1. Timestamps are unique in FLTs;
     * 2. Timestamps are kept in sequence of insertion, and have random access;
     * 3. Timestamps are accessible via values.
     */
    class FLTs : public Times {
        void print(std::ostream & os) const;
        friend std::ostream& operator<<(std::ostream& os, FLTs const & obj);
    };
}

#endif /* TIME_H */

