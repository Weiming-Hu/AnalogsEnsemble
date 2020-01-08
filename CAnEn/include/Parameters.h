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

//#include "By.h"
#include "AnEnDefaults.h"

#ifndef BOOST_NO_AUTO_PTR
#define BOOST_NO_AUTO_PTR
#endif

#include "boost/bimap/vector_of.hpp"
#include "boost/bimap/set_of.hpp"
#include "boost/bimap.hpp"

//#include <boost/multi_index_container.hpp>
//#include <boost/multi_index/random_access_index.hpp>
//#include <boost/multi_index/identity.hpp>
//#include <boost/multi_index/hashed_index.hpp>
//#include <boost/multi_index/tag.hpp>
//#include <boost/multi_index/mem_fun.hpp>

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
     * @param circular Whether parameter is circular.
     */
    Parameter(const Parameter& other);

    /**
     * Initialize parameter.
     * @param name Parameter name.
     * @param weight Parameter weight.
     * @param circular Whether parameter is circular.
     */
    Parameter(std::string name,
            double weight = AnEnDefaults::_WEIGHT,
            bool circular = AnEnDefaults::_CIRCULAR);


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

    void setName(std::string);
    void setWeight(double);
    void setCircular(bool);

    std::string getName() const;
    double getWeight() const;
    bool getCircular() const;

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Parameter const &);

private:
    std::string name_;
    double weight_;
    bool circular_;
};

/**
 * Base class for Parameters
 */
//using multiIndexParameters = boost::multi_index_container<
//        Parameter,
//        boost::multi_index::indexed_by<
//
//        // Order by insertion
//        boost::multi_index::random_access<
//        boost::multi_index::tag<By::insert> >,
//
//        // Access by name
//        boost::multi_index::hashed_non_unique<
//        boost::multi_index::tag<By::name>,
//        boost::multi_index::const_mem_fun<
//        Parameter, std::string, &Parameter::getName> >
//        > >;

using BmParameters = boost::bimap<boost::bimaps::vector_of<size_t>,
        boost::bimaps::set_of<Parameter> >;

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
class Parameters : public BmParameters {
public:

    Parameters();
    virtual ~Parameters();

    /**
     * Gets the parameter by name. If multiple parameters have the same
     * name, this function returns the first parameter found.
     * 
     * @param name The name of the parameter
     * @return Parameter.
     */
//    Parameter const & getParameterByName(std::string name) const;

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Parameters const &);
};

#endif /* PARAMETERS_H */

