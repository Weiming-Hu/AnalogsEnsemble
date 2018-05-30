/* 
 * File:   Parameters.h
 * Author: guido
 *
 * Created on April 18, 2018, 12:18 AM
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <unordered_map>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

class Parameter {
public:
    Parameter();
    Parameter(std::string, double, bool);
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

/**
 * Parameters is a protected inheritance from 
 * std::unordered_map<std::size_t, Parameter> because certain functions, like
 * the insert function, have different behaviors.
 */
class Parameters : protected std::unordered_map<std::size_t, Parameter> {
public:
    Parameters();
    virtual ~Parameters();

    /**
     * Inserts a parameter into the map with the key set to its ID. By this way,
     * it is ensured that parameters are unique in the map with their ID as 
     * the key.
     * 
     * @param station Parameter object.
     * @return std::pair<iterator, bool> object consisting of an iterator to 
     * the inserted element (or to the element that prevented the insertion) 
     * and a bool denoting whether the insertion took place.
     */
    std::pair<iterator, bool> insert(const Parameter & parameter) {
        std::pair<std::size_t, Parameter> p(parameter.getID(), parameter);
        return (std::unordered_map<std::size_t, Parameter>::insert(p));
    };

    size_t size() const _NOEXCEPT;
    void clear() _NOEXCEPT;

    /**
     * Gets the reference to the Parameter based on the ID. If no such 
     * element exists, an exception of type std::out_of_range is thrown.
     * 
     * @param key An size_t key.
     * @return A reference to Parameter object.
     */
    Parameter & at(const size_t & key);
    const Parameter & at(const size_t & key) const;

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Parameters const &);
};

#endif /* PARAMETERS_H */

