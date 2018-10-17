/* 
 * File:   Parameters.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
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
        
        /**
         * Initialize parameter.
         * @param name Parameter name.
         */
        Parameter(std::string name);
        
        /**
         * Initialize parameter.
         * @param name Parameter name.
         * @param weight Parameter weight.
         */
        Parameter(std::string name, double weight);
        
        /**
         * Initialize parameter.
         * @param name Parameter name.
         * @param weight Parameter weight.
         * @param circular Whether parameter is circular.
         */
        Parameter(std::string name, double weight, bool circular);
        
        /**
         * Initialize parameter.
         * @param name Parameter name.
         * @param circular Whether parameter is circular.
         */
        Parameter(std::string name, bool circular);

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
        static std::size_t getStaticID();

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

    struct by_name {
        /** 
         * The tag for name-based indexing
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
            Parameter, std::size_t, &Parameter::getID> >,
            
            // Access by name
            boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<by_name>,
            boost::multi_index::const_mem_fun<
            Parameter, std::string, &Parameter::getName> >
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

        /**
         * Gets the parameter index by ID.
         * 
         * @param parameter_ID The parameter ID.
         * @return The parameter index.
         */
        std::size_t getParameterIndex(std::size_t parameter_ID) const;
        
        /**
         * Gets the parameter by name. If multiple parameters have the same
         * name, this function returns the first parameter found.
         * 
         * @param name The name of the parameter
         * @return anenPar::Parameter.
         */
        Parameter getParameterByName(std::string name) const;

        void print(std::ostream &) const;
        friend std::ostream& operator<<(std::ostream&, Parameters const &);
    };

}
#endif /* PARAMETERS_H */

