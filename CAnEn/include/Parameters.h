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

#include "BmDim.h"
#include "AnEnDefaults.h"

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
class Parameters : public BmType<Parameter> {
public:
    Parameters();
    virtual ~Parameters();

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Parameters const &);
};

#endif /* PARAMETERS_H */

