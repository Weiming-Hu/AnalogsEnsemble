/* 
 * File:   AnEnIO.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 4, 2018, 11:16 AM
 */

#ifndef ANENIO_H
#define ANENIO_H

#include <netcdf>
#include <string>
#include <vector>
#include <cstddef>

#include "Times.h"
#include "Stations.h"
#include "Parameters.h"
#include "Forecasts.h"
#include "Observations.h"
#include "colorTexts.h"

/**
 * \class AnEnIO
 * 
 * \brief AnEnIO class provides interfaces for reading and writing NetCDF files
 * in forecast, observation, and prediction format. These formats are predefined
 * for generating predictions.
 * 
 */
class AnEnIO {
public:
    AnEnIO() = delete;
    AnEnIO(const AnEnIO& orig) = delete;

    AnEnIO(std::string mode, std::string file_path);
    AnEnIO(std::string mode, std::string file_path, std::string file_type);
    AnEnIO(std::string mode, std::string file_path,
            std::string file_type, int verbose);
    AnEnIO(std::string mode, std::string file_path,
            std::string file_type, int verbose,
            std::vector<std::string> required_variables,
            std::vector<std::string> optional_variables);

    virtual ~AnEnIO();

    /**
     * Specifies the return error type of a function. Use 
     * AnEnIO::handleError to handle the returned errorType.
     */
    enum errorType {
        SUCCESS = 0,
        UNKNOWN_ERROR = -1,
        UNKOWN_FILE_TYPE = -2,
        WRONG_FILE_TYPE = -3,
        FILE_EXISTS = -4,
        UNKNOWN_MODE = -5,
        WRONG_MODE = -6,

        FILE_NOT_FOUND = -10,
        REQUIRED_VARIABLE_MISSING = -11,
        OPTIONAL_VARIABLE_MISSING = -12,
        DIMENSION_MISSING = -13,
        WRONG_VARIABLE_SHAPE = -14,
        WRONG_VARIABLE_TYPE = -15,
        ELEMENT_NOT_UNIQUE = -16,
        DIMENSION_EXISTS = -17,
        VARIABLE_EXISTS = -18,
        WRONG_INDEX_SHAPE = -19,

        ERROR_SETTING_VALUES = -50,
        NAN_VALUES = -51,

        INSUFFICIENT_MEMORY = -100
    };

    /**
     * Checks object mode for input/output.
     * 
     * @return AnEnIO::errorType
     */
    errorType checkMode() const;

    /**
     * Checks file path and file extension.
     * 
     * @return AnEnIO::errorType.
     */
    errorType checkFile() const;

    /**
     * Checks whether the NetCDF file conforms with the file type.
     * 
     * The supported file type are
     * - Observations
     * - Forecasts
     * 
     * @return AnEnIO::errorType.
     */
    errorType checkFileType() const;

    /**
     * Checks whether the variable exists.
     * @param var_name The variable name.
     * @param optional Whether the variable is optional.
     * @return AnEnIO::errorType.
     */
    errorType checkVariable(std::string var_name, bool optional) const;

    /**
     * Checks whether the dimension exists.
     * 
     * @param dim_name The dimension name.
     * @return AnEnIO::errorType.
     */
    errorType checkDim(std::string dim_name) const;

    /**
     * Checks the required and optional variables.
     * @return AnEnIO::errorType.
     */
    errorType checkVariables() const;

    /**
     * Checks the required dimensions.
     * @return AnEnIO::errorType.
     */
    errorType checkDimensions() const;

    /**
     * Reads observation file into an Observations object.
     * 
     * @param observations The Observations object to store data.
     * @return An AnEnIO::errorType.
     */
    errorType readObservations(Observations & observations);

    /**
     * Reads a subset of an observation file into an Observations
     * object.
     * 
     * @param observations The Observations object to store data.
     * @param start A vector of indices of the start of the data to read.
     * @param count A vector of numbers of data to read for each dimension.
     * @param stride A vector of numbers of the length of the stride for
     * each dimension.
     * @return An AnEnIO::errorType.
     */
    errorType readObservations(Observations & observations,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1});

    /**
     * Reads forecast file into an Forecasts object.
     * 
     * @param forecasts The Forecasts object to store data.
     * @return An AnEnIO::errorType.
     */
    errorType readForecasts(Forecasts & forecasts);

    /**
     * Reads a subset of a forecast file into a Forecasts object.
     * 
     * @param forecasts The Forecasts object to store data.
     * @param start A vector of indices of the start of the data to read.
     * @param count A vector of numbers of data to read for each dimension.
     * @param stride A vector of numbers of the length of the stride for
     * each dimension.
     * @return An AnEnIO::errorType.
     */
    errorType readForecasts(Forecasts & forecasts,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1, 1});

    /**
     * Reads forecast lead time information from the file. You should use the
     * function AnEnIO::setFilePath first or make sure you have already specify
     * a file to read from using the constructor.
     * 
     * This function assumes the name of the variable is "FLTs".
     * 
     * @param flts The anenTime::FLTs object to store the information.
     * @return An AnEnIO::errorType.
     */
    errorType readFLTs(anenTime::FLTs & flts);
    errorType readFLTs(anenTime::FLTs & flts,
            size_t start, size_t count, ptrdiff_t stride = 1);

    /**
     * Reads parameter information from the file. You should use the function 
     * AnEnIO::setFilePath first or make sure you have already specify a file to
     * read from using the constructor.
     * 
     * This function assumes the dimension name "num_parameters", and the
     * optional variable names "ParameterNames" and "ParameterCirculars".
     * 
     * @param parameters The anenPar::Parameters object to store the information.
     * @return An AnEnIO::errorType.
     */
    errorType readParameters(anenPar::Parameters & parameters);
    errorType readParameters(anenPar::Parameters & parameters,
            size_t start, size_t count, ptrdiff_t stride = 1);

    /**
     * Reads station information from the file. You should use the function 
     * AnEnIO::setFilePath first or make sure you have already specify a file to
     * read from using the constructor.
     * 
     * This function assumes the dimension name "num_stations", and the optional
     * variable names "StationNames", "Xs", and "Ys".
     * 
     * @param stations The anenSta::Stations object to store information.
     * @return An AnEnIO::errorType.
     */
    errorType readStations(anenSta::Stations & stations);
    errorType readStations(anenSta::Stations & stations,
            size_t start, size_t count, ptrdiff_t stride = 1);

    /**
     * Reads time information from the file. You should use the function 
     * AnEnIO::setFilePath first or make sure you have already specify a file to
     * read from using the constructor.
     * 
     * This function assumes the dimension name "Times".
     * 
     * @param times The anenTime::Times object to store information.
     * @return An AnEnIO::errorType.
     */
    errorType readTimes(anenTime::Times & times);
    errorType readTimes(anenTime::Times & times,
            size_t start, size_t count, ptrdiff_t stride = 1);

    /**
     * Reads the length of a dimension.
     * 
     * @param dim_name The name of the dimension to read.
     * @param len The variable to store the length information.
     * @return An AnEnIO::errorType.
     */
    errorType readDimLength(std::string dim_name, std::size_t & len);

    errorType writeForecasts(const Forecasts & forecasts) const;
    errorType writeObservations(const Observations & observations) const;
    errorType writeFLTs(const anenTime::FLTs & flts, bool unlimited) const;
    errorType writeParameters(
            const anenPar::Parameters & parameters, bool unlimited) const;
    errorType writeStations(
            const anenSta::Stations & stations, bool unlimited) const;
    errorType writeTimes(const anenTime::Times & times, bool unlimited) const;

    /**
     * Handles the errorType variable.
     * 
     * @param indicator An AnEnIO::errorType item.
     */
    void handleError(const errorType & indicator) const;

    int getVerbose() const;
    std::string getMode() const;
    std::string getFilePath() const;
    std::string getFileType() const;
    std::vector<std::string> getOptionalVariables() const;
    std::vector<std::string> getRequiredVariables() const;
    std::vector<std::string> getRequiredDimensions() const;

    void setVerbose(int verbose_);
    void setMode(std::string mode);
    void setMode(std::string mode, std::string file_path);
    void setFilePath(std::string file_path);
    void setFileType(std::string file_type);
    void setOptionalVariables(std::vector<std::string> optional_variables);
    void setRequiredVariables(std::vector<std::string> required_variables);
    void setRequiredDimensions(std::vector<std::string> required_dimensions);

    /**
     * Dumps the values of a variable to the standard output.
     * 
     * @param var_name The name of variable to print.
     * @param start The start position, starting from 0.
     * @param count The number of values to print.
     */
    void dumpVariable(std::string var_name,
            std::size_t start = 0,
            std::size_t count = 0) const;


protected:

    /**
     * Specifies the mode of the object. Values can be:
     * - Read
     * - Write
     */
    std::string mode_;

    /**
     * Specifies the full path to the file.
     */
    std::string file_path_;

    /**
     * Specifies the type of the file. Values can be:
     * - Observations
     * - Forecasts
     */
    std::string file_type_;

    /**
     * Specifies the verbose level.
     * - 0: Quiet.
     * - 1: Errors.
     * - 2: The above plus Warnings.
     * - 3: The above plus program progress information.
     * - 4: The above plus session information.
     */
    int verbose_ = 2;

    /**
     * Specifies the required variable names in the file.
     */
    std::vector<std::string> required_variables_;

    /**
     * Specifies the optional variable names in the file.
     */
    std::vector<std::string> optional_variables_;

    /**
     * Specifies the required dimension names in the file.
     */
    std::vector<std::string> required_dimensions_;

    /**
     * Reads variables as a string vector. The variable should has two 
     * dimensions and be the type of nc_CHAR.
     * 
     * @param var_name The variable name.
     * @param results A string container.
     * @return AnEnIO::errorType.
     */
    errorType read_string_vector_(std::string var_name,
            std::vector<std::string> & results) const;
    errorType read_string_vector_(std::string var_name,
            std::vector<std::string> & results,
            size_t start, size_t count, ptrdiff_t stride = 1) const;

    /**
     * Cleans special characters in strings.
     * 
     * @param str A string.
     */
    void purge_(std::string & str) const;

    /**
     * Cleans special characters in strings
     * 
     * @param strs A vector of strings.
     */
    void purge_(std::vector<std::string> & strs) const;

    /************************************************************************
     *                      Template Functions                              *
     ************************************************************************/

    /**
     * Reads variables as a atomic vector.
     * 
     * @param var_name The variable name.
     * @param results An atmoic container.
     * @return AnEnIO::errorType.
     */
    template<typename T>
    errorType
    read_vector_(std::string var_name, std::vector<T> & results) const {

        using namespace netCDF;
        using namespace std;

        if (mode_ != "Read") {
            if (verbose_ >= 1) cout << "Error: Mode should be 'Read'." << endl;
            return (WRONG_MODE);
        }

        NcFile nc(file_path_, NcFile::FileMode::read);
        NcVar var = nc.getVar(var_name);
        auto var_dims = var.getDims();
        T *p_vals = nullptr;

        size_t len = 1;
        for (const auto & dim : var_dims) {
            len *= dim.getSize();
        }

        try {
            p_vals = new T[len];
        } catch (bad_alloc & e) {
            nc.close();
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Insufficient memory when reading variable ("
                    << var_name << ")!" << RESET << endl;
            nc.close();
            return (INSUFFICIENT_MEMORY);
        }

        var.getVar(p_vals);
        results.assign(p_vals, p_vals + len);

        nc.close();
        delete [] p_vals;
        return (SUCCESS);
    };

    template<typename T>
    errorType
    read_vector_(std::string var_name, std::vector<T> & results,
            std::vector<size_t> start, std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1}) const {

        using namespace netCDF;
        using namespace std;

        if (mode_ != "Read") {
            if (verbose_ >= 1) cout << "Error: Mode should be 'Read'." << endl;
            return (WRONG_MODE);
        }

        NcFile nc(file_path_, NcFile::FileMode::read);
        NcVar var = nc.getVar(var_name);
        T *p_vals = nullptr;

        size_t total = 1;
        for (auto i : count) {
            total *= i;
        }

        try {
            p_vals = new T[total];
        } catch (bad_alloc & e) {
            nc.close();
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Insufficient memory when reading variable ("
                    << var_name << ")!" << RESET << endl;
            nc.close();
            return (INSUFFICIENT_MEMORY);
        }

        // Reverse the order because of the storage style of NetCDF
        reverse(start.begin(), start.end());
        reverse(count.begin(), count.end());
        reverse(stride.begin(), stride.end());

        var.getVar(start, count, stride, p_vals);
        results.assign(p_vals, p_vals + total);

        nc.close();
        delete [] p_vals;
        return (SUCCESS);
    };

    template<typename T>
    errorType
    read_vector_(std::string var_name, std::vector<T> & results,
            size_t start, size_t count, ptrdiff_t stride = 1) const {
        std::vector<size_t> vec_start{start}, vec_count{count};
        std::vector<ptrdiff_t> vec_stride{stride};
        return (read_vector_(var_name, results,
                vec_start, vec_count, vec_stride));
    };

};

#endif /* ANENIO_H */