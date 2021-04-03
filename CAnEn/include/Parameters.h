/* 
 * File:   Parameters.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 *
 * Created on April 18, 2018, 12:18 AM
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <iostream>

#include "BmDim.h"
#include "Config.h"

/**
 * \class Parameter
 * 
 * \brief Parameter stores parameter information including name and circular.
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
     * @param circular Whether parameter is circular.
     */
    Parameter(std::string name,
            bool circular = Config::_CIRCULAR);


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
    void setCircular(bool);

    std::string getName() const;
    bool getCircular() const;

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Parameter const &);

private:
    std::string name_;
    bool circular_;
};

/**
 * \class Parameters
 * 
 * \brief Parameters class stores Parameter objects. It is a bidirectional map
 * implemented from Boost so that it provides fast translation from and to 
 * its underlying Time object.
 * 
 * Parameters class support the following features:
 * 1. Parameter is unique.
 * 2. Parameter objects are kept in sequence of insertion and have random access.
 * 3. Index of a Parameter object can be quickly retrieved using Parameter.
 */
class Parameters : public BmType<Parameter> {
public:
    Parameters() = default;
    virtual ~Parameters() = default;

    void push_back(const Parameter &);

    std::size_t getIndex(const Parameter &) const;
    void getIndices(const Parameters &, std::vector<std::size_t> &) const;
    
    const Parameter & getParameter(std::size_t index) const;

    void getCirculars(std::vector<bool> & circulars) const;
    void getNames(std::vector<std::string> & names) const;

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Parameters const &);
};

#endif /* PARAMETERS_H */

