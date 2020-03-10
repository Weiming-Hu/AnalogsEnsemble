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

/**
 * \class ParameterGrib
 * 
 * \brief ParameterGrib stores parameter information including the extra
 * information, e.g. ID, variable level, and the type of variable level.
 * These information is very helpful when locating a specific variable
 * in a Grib2 format file.
 *
 * To find any supported variables, please use the database provided
 * by ECMWF Eccodes:
 *
 * https://apps.ecmwf.int/codes/grib/param-db/
 *
 * To find the associated arguments for a variable, first identify the
 * variable ID from the database, and then use the following eccodes
 * commandline tool to find out what the level and the type of level
 * are. For example, for the following variable, 
 *
 * https://apps.ecmwf.int/codes/grib/param-db/?id=132
 *
 * the variable ID is 132. To show all the levels and types of levels
 * contained in a specific file, you can use the command line tool:
 *
 * $ grib_ls -w paramId=132 -p paramId,level,typeOfLevel,shortName,name nam_218_20160101_0000_000.grb2 
 * nam_218_20160101_0000_000.grb2
 * paramId      level        typeOfLevel  shortName    name         
 * 132          50           isobaricInhPa  v            V component of wind 
 * 132          75           isobaricInhPa  v            V component of wind 
 * 132          100          isobaricInhPa  v            V component of wind 
 * 132          125          isobaricInhPa  v            V component of wind 
 * 132          150          isobaricInhPa  v            V component of wind 
 * 132          175          isobaricInhPa  v            V component of wind 
 * 132          200          isobaricInhPa  v            V component of wind 
 * 132          225          isobaricInhPa  v            V component of wind 
 * 132          250          isobaricInhPa  v            V component of wind 
 * 132          275          isobaricInhPa  v            V component of wind 
 * 132          300          isobaricInhPa  v            V component of wind 
 * 132          325          isobaricInhPa  v            V component of wind 
 * 132          350          isobaricInhPa  v            V component of wind 
 * 132          375          isobaricInhPa  v            V component of wind 
 * 132          400          isobaricInhPa  v            V component of wind 
 * 132          425          isobaricInhPa  v            V component of wind 
 * 132          450          isobaricInhPa  v            V component of wind 
 * 132          475          isobaricInhPa  v            V component of wind 
 * 132          500          isobaricInhPa  v            V component of wind 
 * 132          525          isobaricInhPa  v            V component of wind 
 * 132          550          isobaricInhPa  v            V component of wind 
 * 132          575          isobaricInhPa  v            V component of wind 
 * 132          600          isobaricInhPa  v            V component of wind 
 * 132          625          isobaricInhPa  v            V component of wind 
 * 132          650          isobaricInhPa  v            V component of wind 
 * 132          675          isobaricInhPa  v            V component of wind 
 * 132          700          isobaricInhPa  v            V component of wind 
 * 132          725          isobaricInhPa  v            V component of wind 
 * 132          750          isobaricInhPa  v            V component of wind 
 * 132          775          isobaricInhPa  v            V component of wind 
 * 132          800          isobaricInhPa  v            V component of wind 
 * 132          825          isobaricInhPa  v            V component of wind 
 * 132          850          isobaricInhPa  v            V component of wind 
 * 132          875          isobaricInhPa  v            V component of wind 
 * 132          900          isobaricInhPa  v            V component of wind 
 * 132          925          isobaricInhPa  v            V component of wind 
 * 132          950          isobaricInhPa  v            V component of wind 
 * 132          975          isobaricInhPa  v            V component of wind 
 * 132          1000         isobaricInhPa  v            V component of wind 
 * 132          3000         pressureFromGroundLayer  v            V component of wind 
 * 132          6000         pressureFromGroundLayer  v            V component of wind 
 * 132          9000         pressureFromGroundLayer  v            V component of wind 
 * 132          12000        pressureFromGroundLayer  v            V component of wind 
 * 132          15000        pressureFromGroundLayer  v            V component of wind 
 * 132          18000        pressureFromGroundLayer  v            V component of wind 
 * 132          0            tropopause   v            V component of wind 
 * 132          0            maxWind      v            V component of wind 
 * 132          0            unknown      v            V component of wind 
 * 132          80           heightAboveGround  v            V component of wind 
 * 49 of 445 messages in nam_218_20160101_0000_000.grb2
 * 
 * 49 of 445 total messages in 1 files
 *
 * In this way, you can locate any variables by a combination of variable ID, level, and 
 * the type of the variable level.
 *
 * If you want to find some quick test data files, you can access them here.
 *
 * https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/tests/Data
 */
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

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, ParameterGrib const &);

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

