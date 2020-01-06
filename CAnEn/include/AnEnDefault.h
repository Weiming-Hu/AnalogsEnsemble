/* 
 * File:   Default.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 5, 2020, 7:27 AM
 */

#ifndef ANENDEFAULT_H
#define ANENDEFAULT_H

#include <string>
#include <cmath>

/**
 * The AnEnDefault namespace includes default values used throughout the project.
 */
namespace AnEnDefaults {
    static const std::string _NAME = "UNDEFINED";
    static const double _WEIGHT = 1;
    static const bool _CIRCULAR = false;
    static const double _X = 0.0;
    static const double _Y = 0.0;
    static const std::string _UNIT = "seconds";
    static const std::string _ORIGIN = "1970-01-01";
}

#endif /* ANENDEFAULT_H */

