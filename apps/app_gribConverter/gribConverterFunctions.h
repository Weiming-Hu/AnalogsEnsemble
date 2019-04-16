/* 
 * File:   gribConverterFunctions.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 17, 2018, 5:20 PM
 */

/** @file */


#ifndef GRIBCONVERTERFUNCTIONS_H
#define GRIBCONVERTERFUNCTIONS_H

#include "eccodes.h"
#include "AnEn.h"
#include "AnEnIO.h"
#include "colorTexts.h"
#include "CommonExeFunctions.h"

#include "boost/filesystem.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/program_options/options_description.hpp"
#include "boost/program_options/parsers.hpp"
#include "boost/program_options/variables_map.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <cassert>
#include <exception>
#include <algorithm>

namespace gribConverter {

    using namespace std;
    namespace fs = boost::filesystem;

    const static int _SECS_IN_DAY = 24 * 60 * 60;

    /**
     * getXY gets the xs and ys of a parameter.
     * 
     * @param xs A vector to store the x values.
     * @param ys A vector to store the y values.
     * @param file The file to read data from.
     * @param par_id The ID of the parameter. This can be found using the 
     * grib_ls utility or by looking up in the ecCodes parameter database,
     * \link{http://apps.ecmwf.int/codes/grib/param-db/}.
     * @param level The level of the parameter.
     * @param par_key The key name of the parameter.
     * @param level_key The key name of the level.
     * @param val_key The key name of the value.
     */
    void getXY(vector<double> & xs, vector<double> & ys, string file, long par_id,
            long level, string par_key = "paramId", string level_key = "level",
            string val_key = "values");

    /**
     * getDoubles gets an array of values based on the paramId and the level.
     * The values are in row-major as the convention of C++.
     * 
     * @param vals A vector to store the data array.
     * @param file The file to read data from.
     * @param par_id The ID of the parameter. This can be found using the 
     * grib_ls utility or by looking up in the ecCodes parameter database,
     * \link{http://apps.ecmwf.int/codes/grib/param-db/}.
     * @param level The level of the parameter.
     * @param type The type for level (typeOfLevel)
     * @param par_key The key name of the parameter.
     * @param level_key The key name of the level.
     * @param type_key The key name of the typeOfLevel.
     * @param val_key The key name of the value.
     */
    void getDoubles(vector<double> & vals, const string & file, long par_id, long level,
            const string & type, const string & par_key = "parameter", const string & level_key = "level",
            const string & type_key = "typeOfLevel", const string & val_key = "values");

    /**
     * toForecasts function reads data multiple forecast files and write them in
     * Forecasts NetCDf file that is ready to be used to compute Analog Ensembles.
     * 
     * Parameter and Station information are determined from the data in file. Parameter
     * is determined by the combination of the parameter ID and the level number. Station
     * is determined by the x (or lon) and y (or lat) variable.
     * 
     * FLT and Time information are determined from the file names using regular expressions.
     * The first variable matched in the regular expression will be used. For example,
     * we have a regular expression like this
     *         '.*nam_218_\d{8}_\d{4}_(\d{3})\.grb2$'
     * 
     * This expression has one extract variable '(\d{3})', and therefore the string
     * matched here will be used to calculate the FLT or Time information. The Time information
     * will be determined by calculating the duration in second from the original date.
     * 
     * @param files_in A vector of file names that are found in the folder. The folder
     * is provided in the program option.
     * @param file_out The output file path.
     * @param pars_id A vector of parameters ID that will be read from each file.
     * @param pars_new_name A vector of names that will assign to the parameters in the new
     * Forecasts NetCDF file.
     * @param crcl_pars_id A vector of parameters ID for circular parameters.
     * @param levels A vector of level numbers to specify for each parameter.
     * @param level_types A vector of types of levels.
     * @param par_key The key name of parameter ID.
     * @param level_key The key name of level.
     * @param type_key The key name of type of level.
     * @param val_key The key name of values.
     * @param regex_time_str The regular expression used to extract time information.
     * @param regex_flt_str The regular expression used to extract FLT information.
     * @param flt_interval The FLT interval in seconds. This interval will be used to multiply
     * the FLT information extracted from the file name to determine the actual offset in seconds
     * for each FLT timestamp from the origin.
     * @param delimited Whether the extract Time information is delimited. A string like
     * '19900701' will be undelimited; a string like '1990-07-01' or '1990/07/01' is delimited.
     * @param verbose An integer for the verbose level.
     */
    void toForecasts(vector<string> & files_in, const string & file_out,
            const vector<long> & pars_id, const vector<string> & pars_new_name,
            const vector<long> & crcl_pars_id, const vector<long> & levels,
            const vector<string> & level_types,
            const string & par_key, const string & level_key,
            const string & type_key, const string & val_key,
            string regex_time_str, string regex_flt_str,
            double flt_interval, bool delimited = false,
            bool skip_data = false, int verbose = 0);

    /**
     * toObservations function reads data from multiple observation files and write them 
     * in Observations NetCDF fiel that is ready to be used to compute Analog Ensembles.
     * 
     * Parameter and Station information are determined from the data in file. Parameter
     * is determined by the combination of the parameter ID and the level number. Station
     * is determined by the x (or lon) and y (or lat) variable.
     * 
     * Time information are determined from the file names using regular expressions.
     * The first variable matched in the regular expression will be used. For example,
     * we have a regular expression like this
     *         '.*nam_218_\d{8}_(\d{4})_\d{3}\.grb2$'
     * 
     * This expression has one extract variable '(\d{4})', and therefore the string
     * matched here will be used to calculate the Time information. The Time information
     * will be determined by calculating the duration in second from the original date.
     * 
     * @param files_in A vector of file names that are found in the folder. The folder
     * is provided in the program option.
     * @param file_out The output file path.
     * @param pars_id A vector of parameters ID that will be read from each file.
     * @param pars_new_name A vector of names that will assign to the parameters in the new
     * Forecasts NetCDF file.
     * @param crcl_pars_id A vector of parameters ID for circular parameters.
     * @param levels A vector of level numbers to specify for each parameter.
     * @param level_types A vector of types of levels.
     * @param par_key The key name of parameter ID.
     * @param level_key The key name of level.
     * @param type_key The key name of type of level.
     * @param val_key The key name of values.
     * @param regex_time_str The regular expression used to extract time information.
     * @param delimited Whether the extract Time information is delimited. A string like
     * '19900701' will be undelimited; a string like '1990-07-01' or '1990/07/01' is delimited.
     * @param verbose An integer for the verbose level.
     */
    void toObservations(vector<string> & files_in, const string & file_out,
            const vector<long> & pars_id, const vector<string> & pars_new_name,
            const vector<long> & crcl_pars_id, const vector<long> & levels,
            const vector<string> & level_types,
            const string & par_key, const string & level_key,
            const string & type_key, const string & val_key,
            string regex_time_str, bool delimited = false,
            bool skip_data = false, int verbose = 0);

    void query_grib_eccodes_(
            string file, long par_id, long level, string type,
            string par_key, string level_key, string type_key,
            codes_index** p_index, int* p_ret);
}

#endif
