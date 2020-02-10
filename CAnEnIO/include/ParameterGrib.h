/* 
 * File:   ParameterGrib.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 7, 2020, 5:57 PM
 */

#ifndef PARAMETERGRIB_H
#define PARAMETERGRIB_H

#include <iostream>
#include "Parameters.h"

using namespace std;

class ParameterGrib : public Parameter {
public:
    ParameterGrib();
    ParameterGrib(const ParameterGrib &);
    ParameterGrib(string name, 
            bool circular = Config::_CIRCULAR,
            long id = 0, long level = 0,
            std::string level_type = "UNDEFINED");

    virtual ~ParameterGrib();

    long getId() const;
    long getLevel() const;
    std::string getLevelType() const;

    ParameterGrib & operator=(const ParameterGrib &);
    bool operator<(const ParameterGrib &) const;

    static const std::string _key_id;
    static const std::string _key_level;
    static const std::string _key_level_type;
    static const std::string _key_values;

private:
    long id_;
    long level_;
    std::string level_type_;
};

#endif /* PARAMETERGRIB_H */

