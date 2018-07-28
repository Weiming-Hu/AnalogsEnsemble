/* 
 * File:   Parameters.h
 * Author: guido
 *
 * Created on April 18, 2018, 12:18 AM
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/mem_fun.hpp>

/**
 * The anenPar name space is created for classes Parameters and Parameter.
 */
namespace anenPar {

    /**
     * \class Parameter
     * 
     * \brief Parameter stores parameter information including name, weight, and
     * circular. Each Parameter object is assigned with an unique ID. This ID
     * can be useful for parameter retrieval.
     * 
     */
    class Parameter {
    public:
        Parameter();
        Parameter(std::string);
        Parameter(std::string, double);
        Parameter(std::string, double, bool);
        Parameter(std::string, bool);
        Parameter(const Parameter& other);

        virtual ~Parameter();

        Parameter& operator=(const Parameter& rhs);
        bool operator==(const Parameter& rhs) const;

        /**
         * Compares two Parameter objects based solely on name. The comparison
         * algorithm uses std::string::compare.
         * 
         * @param right Right hand-side of the compare sign.
         * @return A boolean for whether this < right.
         */
        bool operator<(const Parameter& right) const;
        bool compare(const Parameter & rhs) const;

        void setName(std::string);
        void setWeight(double);
        void setCircular(bool);

        std::string getName() const;
        double getWeight() const;
        bool getCircular() const;
        std::size_t getID() const;

        void print(std::ostream &) const;
        friend std::ostream& operator<<(std::ostream&, Parameter const &);

    private:
        void setID_();

        /**
         * This is a unique identifier that is used to keep track of parameters.
         */
        std::size_t ID_;

        std::string name_ = "UNDEFINED";
        double weight_ = NAN;
        bool circular_ = false;

        /**
         * Static variable for serial number identification.
         */
        static size_t _static_ID_;
    };

    struct by_insert {
        /**
         * The tag for insertion sequence indexing
         */
    };

    struct by_ID {
        /** 
         * The tag for ID-based indexing
         */
    };

    /**
     * Base class for Parameters
     */
    using multiIndexParameters = boost::multi_index_container<
            Parameter,
            boost::multi_index::indexed_by<

            // Order by insertion
            boost::multi_index::random_access<
            boost::multi_index::tag<by_insert> >,

            // Order by ID
            boost::multi_index::hashed_unique<
            boost::multi_index::tag<by_ID>,
            boost::multi_index::const_mem_fun<
            Parameter, std::size_t, &Parameter::getID> >
            > >;

    /**
     * \class Parameters
     * 
     * \brief Parameters class stores Parameter objects.
     * 
     * Parameters class support the following features:
     * 1. Parameter should be unique in Parameters;
     * 2. Parameter objects are kept in sequence of insertion, and 
     * has random access;
     * 3. Parameter are accessible via Parameter ID.
     */
    class Parameters : public multiIndexParameters {
    public:
        Parameters();
        virtual ~Parameters();

        std::size_t getParameterIndex(std::size_t parameter_ID) const;

        void print(std::ostream &) const;
        friend std::ostream& operator<<(std::ostream&, Parameters const &);
    };

}
#endif /* PARAMETERS_H */

