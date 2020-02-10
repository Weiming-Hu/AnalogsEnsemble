/*
 * File:   ParameterGrib.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 7, 2020, 5:57 PM
 */

#include "ParameterGrib.h"

using namespace std;

const string ParameterGrib::_key_id = "parameter";
const string ParameterGrib::_key_level = "level";
const string ParameterGrib::_key_level_type = "typeOfLevel";
const string ParameterGrib::_key_values = "values";

ParameterGrib::ParameterGrib() : Parameter::Parameter() {
}

ParameterGrib::ParameterGrib(const ParameterGrib & orig) :
Parameter::Parameter(orig) {
    id_ = orig.id_;
    level_ = orig.level_;
    level_type_ = orig.level_type_;
}

ParameterGrib::ParameterGrib(string name, bool circular,
        long id, long level, string level_type) :
Parameter::Parameter(name, circular),
id_(id), level_(level), level_type_(level_type) {
}

ParameterGrib::~ParameterGrib() {
}

long
ParameterGrib::getId() const {
    return id_;
}

long
ParameterGrib::getLevel() const {
    return level_;
}

string
ParameterGrib::getLevelType() const {
    return level_type_;
}

ParameterGrib &
ParameterGrib::operator=(const ParameterGrib & rhs) {
    if (this != &rhs) {
        Parameter::operator=(rhs);

        id_ = rhs.id_;
        level_ = rhs.level_;
        level_type_ = rhs.level_type_;
    }

    return *this;
}

bool
ParameterGrib::operator<(const ParameterGrib & rhs) const {
    if (id_ != rhs.id_) return id_ < rhs.id_;
    else return level_ < rhs.level_;
}
