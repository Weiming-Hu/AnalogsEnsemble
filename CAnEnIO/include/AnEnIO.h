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
#include "Analogs.h"
#include "Observations.h"
#include "colorTexts.h"
#include "SimilarityMatrices.h"
#include "StandardDeviation.h"
#include "boost/numeric/ublas/matrix.hpp"

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
        /// 0
        SUCCESS = 0,
        /// -1
        UNKNOWN_ERROR = -1,
        /// -2
        UNKOWN_FILE_TYPE = -2,
        /// -3
        WRONG_FILE_TYPE = -3,
        /// -4
        FILE_EXISTS = -4,
        /// -5
        UNKNOWN_MODE = -5,
        /// -6
        WRONG_MODE = -6,
        /// -7
        FILEIO_ERROR = -7,
        /// -8
        METHOD_NOT_IMPLEMENTED = -8,

        /// -10
        FILE_NOT_FOUND = -10,
        /// -11
        REQUIRED_VARIABLE_MISSING = -11,
        /// -12
        OPTIONAL_VARIABLE_MISSING = -12,
        /// -13
        DIMENSION_MISSING = -13,
        /// -14
        WRONG_VARIABLE_SHAPE = -14,
        /// -15
        WRONG_VARIABLE_TYPE = -15,
        /// -16
        ELEMENT_NOT_UNIQUE = -16,
        /// -17
        DIMENSION_EXISTS = -17,
        /// -18
        VARIABLE_EXISTS = -18,
        /// -19
        WRONG_INDEX_SHAPE = -19,

        /// -50
        ERROR_SETTING_VALUES = -50,
        /// -51
        NAN_VALUES = -51,

        /// -51
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
    errorType checkFilePath() const;

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
     * @param observations The Observations_array object to store data.
     * @return An AnEnIO::errorType.
     */
    errorType readObservations(Observations_array & observations) const;

    /**
     * Reads a subset of an observation file into an Observations
     * object.
     * 
     * @param observations The Observations_array object to store data.
     * @param start A vector of indices of the start of the data to read.
     * @param count A vector of numbers of data to read for each dimension.
     * @param stride A vector of numbers of the length of the stride for
     * each dimension.
     * @return An AnEnIO::errorType.
     */
    errorType readObservations(Observations_array & observations,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1}) const;

    /**
     * Reads forecast file into an Forecasts object.
     * 
     * @param forecasts The Forecasts_array object to store data.
     * @return An AnEnIO::errorType.
     */
    errorType readForecasts(Forecasts_array & forecasts) const;

    /**
     * Reads a subset of a forecast file into a Forecasts object.
     * 
     * @param forecasts The Forecasts_array object to store data.
     * @param start A vector of indices of the start of the data to read.
     * @param count A vector of numbers of data to read for each dimension.
     * @param stride A vector of numbers of the length of the stride for
     * each dimension.
     * @return An AnEnIO::errorType.
     */
    errorType readForecasts(Forecasts_array & forecasts,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1, 1}) const;

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
    errorType readFLTs(anenTime::FLTs & flts) const;
    errorType readFLTs(anenTime::FLTs & flts,
            size_t start, size_t count, ptrdiff_t stride = 1) const;

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
    errorType readParameters(anenPar::Parameters & parameters) const;
    errorType readParameters(anenPar::Parameters & parameters,
            size_t start, size_t count, ptrdiff_t stride = 1) const;

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
    errorType readStations(anenSta::Stations & stations,
            const std::string & dim_name_prefix = "",
            const std::string & var_name_prefix = "") const;
    errorType readStations(anenSta::Stations & stations,
            size_t start, size_t count, ptrdiff_t stride = 1,
            const std::string & dim_name_prefix = "",
            const std::string & var_name_prefix = "") const;

    /**
     * Reads time information from the file. You should use the function 
     * AnEnIO::setFilePath first or make sure you have already specify a file to
     * read from using the constructor.
     * 
     * This function assumes the dimension name "Times".
     * 
     * @param times The anenTime::Times object to store information.
     * @param var_name The variable name of the Time variable to be written.
     * @return An AnEnIO::errorType.
     */
    errorType readTimes(anenTime::Times & times,
            const std::string & var_name = "Times") const;
    errorType readTimes(anenTime::Times & times,
            size_t start, size_t count, ptrdiff_t stride = 1,
            const std::string & var_name = "Times") const;

    /**
     * Reads the length of a dimension.
     * 
     * @param dim_name The name of the dimension to read.
     * @param len The variable to store the length information.
     * @return An AnEnIO::errorType.
     */
    errorType readDimLength(std::string dim_name, std::size_t & len) const;

    /**
     * Write forecasts.
     * @param forecasts Forecasts to write.
     * @return An AnEnIO::errorType.
     */
    errorType writeForecasts(const Forecasts & forecasts) const;

    /**
     * Write observations.
     * @param observations Observations to write.
     * @return An AnEnIO::errorType.
     */
    errorType writeObservations(const Observations & observations) const;

    /**
     * Write Forecast Lead Times anenTime::FLTs. A dimension (num_flts)
     * will be created; Variables for FLTs will be created.
     * @param flts anenTime::FLTs to write.
     * @param unlimited Whether this dimension is unlimited.
     * @return An AnEnIO::errorType.
     */
    errorType writeFLTs(const anenTime::FLTs & flts, bool unlimited) const;

    /**
     * Write anenPar::Parameters. A dimension (num_parameters) will be created;
     * Variables for "ParameterNames", "ParameterWeights", "ParameterCirculars"
     * will be created.
     * @param parameters anenPar::Parameters to write.
     * @param unlimited Whether this dimension is unlimited.
     * @return An AnEnIO::errorType.
     */
    errorType writeParameters(
            const anenPar::Parameters & parameters, bool unlimited) const;

    /**
     * Write anenSta::Stations. A dimension (num_stations) will be created;
     * Variables for name of stations, Xs, and Ys will be created.
     * @param stations anenSta::Stations to write.
     * @param unlimited Whether this dimension is unlimited.
     * @return An AnEnIO::errorType.
     */
    errorType writeStations(
            const anenSta::Stations & stations, bool unlimited,
            const std::string & dim_name_prefix = "",
            const std::string & var_name_prefix = "") const;

    /**
     * Write anenTime::Times. A dimension (num_times) will be created; Variables
     * for Times will be created.
     * @param times anenTime::Times to write.
     * @param unlimited Whether this dimension is unlimited.
     * @param dim_name The dimension name associated with the time variable.
     * @param var_name The name of the time variable to be written.
     * @return An AnEnIO::errorType.
     */
    errorType writeTimes(const anenTime::Times & times, bool unlimited,
            const std::string & dim_name = "num_times",
            const std::string & var_name = "Times") const;

    /**
     * Read SimilarityMatrices.
     * @param sims SimilarityMatrices to store data.
     * @return An AnEnIO::errorType.
     */
    errorType readSimilarityMatrices(SimilarityMatrices & sims);

    /**
     * Write SimilarityMatrices. There is a standard requirement for variables
     * that should be included in Similarity file type. Please refer to
     * https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html
     * 
     * @param sims SimilarityMatrices.
     * @param parameters anenPar::Parameters.
     * @param test_stations anenSta::Stations.
     * @param test_times anenTime::Times.
     * @param flts anenTime::FLTs.
     * @param search_stations anenSta::Stations.
     * @param search_times anenTime::Times.
     * @return An AnEnIO::errorType.
     */
    errorType writeSimilarityMatrices(
            const SimilarityMatrices & sims,
            const anenPar::Parameters & parameters,
            const anenSta::Stations & test_stations,
            const anenTime::Times & test_times,
            const anenTime::FLTs & flts,
            const anenSta::Stations & search_stations,
            const anenTime::Times & search_times) const;

    /**
     * Read Analogs.
     * @param analogs Analogs to store the data.
     * @return An AnEnIO::errorType.
     */
    errorType readAnalogs(Analogs & analogs);

    /**
     * Write Analogs. There is a standard requirement for variables
     * that should be included in Analogs file type. Please refer to
     * https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html
     * 
     * @param analogs Analogs to write.
     * @param test_stations anenSta::Stations.
     * @param test_times anenTime::Times.
     * @param flts anenTime::FLTs.
     * @param search_stations anenSta::Stations.
     * @param search_times anenTime::Times.
     * @return An AnEnIO::errorType.
     */
    errorType writeAnalogs(
            const Analogs & analogs,
            const anenSta::Stations & test_stations,
            const anenTime::Times & test_times,
            const anenTime::FLTs & flts,
            const anenSta::Stations & search_stations,
            const anenTime::Times & search_times) const;

    /**
     * Read StandardDeviation.
     * 
     * @param sds StandardDeviation.
     * @return An AnEnIO::errorType. 
     */
    errorType readStandardDeviation(StandardDeviation & sds);
    errorType readStandardDeviation(StandardDeviation & sds,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1});

    /**
     * Write StandardDeviation. There is a standard requirement for variables
     * that should be included in StandardDeviation file type. Please refer to
     * https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html
     * 
     * @param sds StandardDeviation.
     * @param parameters anenPar::Parameters. Parameters are required because
     * there can be circular parameters that must be identified.
     * @param stations anenSta::Stations.
     * @param flts anenTime::FLTs.
     * @return AnEnIO::errorType.
     */
    errorType writeStandardDeviation(
            const StandardDeviation & sds,
            const anenPar::Parameters & parameters,
            const anenSta::Stations & stations,
            const anenTime::FLTs & flts) const;

    /**
     * Handles the errorType variable.
     * 
     * @param indicator An AnEnIO::errorType item.
     */
    void handleError(const errorType & indicator) const;

    /**
     * Whether the write function should be writing to an existing file.
     * 
     * @return A boolean.
     */
    bool isAdd() const;

    /**
     * Get the verbose level.
     * @return Verbose level.
     */
    int getVerbose() const;

    /**
     * Get the mode.
     * @return Mode string.
     */
    std::string getMode() const;

    /**
     * Get file path.
     * @return File path string.
     */
    std::string getFilePath() const;

    /**
     * Get the file type.
     * @return File type string
     */
    std::string getFileType() const;

    /**
     * Get the optional variables to check.
     * @return A vector of optional variables.
     */
    std::vector<std::string> getOptionalVariables() const;

    /**
     * Get the required variables to check.
     * @return A vector of required variables.
     */
    std::vector<std::string> getRequiredVariables() const;

    /**
     * Get the required dimensions to check.
     * @return A vector of required dimensions.
     */
    std::vector<std::string> getRequiredDimensions() const;

    /**
     * Whether the write function should be writing to an existing file.
     * 
     * @param add A boolean
     */
    void setAdd(bool add);

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

    /**
     * Reads the matrix of type boost::numeric::ublas::matrix<T>.
     * Currently AnEn::TimeMapMatrix and AnEn::SearchStationMatrix are using
     * this template.
     * 
     * @param matrix A boost matrix to store the data.
     * @return An AnEnIO::errorType.
     */
    template <typename T>
    errorType
    readTextMatrix(boost::numeric::ublas::matrix<T> & mapping);

    /**
     * Writes the matrix of type boost::numeric::ublas::matrix<T> into a file.
     * Currently AnEn::TimeMapMatrix and AnEn::SearchStationMatrix are using
     * this template.
     * 
     * @param mapping A boost matrix to write.
     * @return An AnEnIO::errorType.
     */
    template <typename T>
    errorType
    writeTextMatrix(const boost::numeric::ublas::matrix<T> & mapping) const;

    static errorType
    combineParameters(
            const std::vector<std::string> & in_files,
            const std::string & file_type,
            anenPar::Parameters & parameters, int verbose);

    static errorType
    combineStations(
            const std::vector<std::string> & in_files,
            const std::string & file_type,
            anenSta::Stations & stations, int verbose);


    static errorType
    combineTimes(
            const std::vector<std::string> & in_files,
            const std::string & file_type,
            anenTime::Times & times, int verbose);

    static errorType
    combineFLTs(
            const std::vector<std::string> & in_files,
            const std::string & file_type,
            anenTime::FLTs & flts, int verbose);

    /**
     * Binds a vector of Forecasts_array.
     * 
     * @param in_files A vector of input Forecasts files.
     * @param forecasts A Forecasts_arary to store the binded sub arrays.
     * @param along Which dimension to append counting from 0.
     * @param verbose Verbose level.
     * @return An AnEnIO::errorType;
     */
    static errorType
    combineForecastsArray(
            const std::vector<std::string> & in_files,
            Forecasts_array & forecasts, size_t along, int verbose = 2);

    /**
     * Binds a vector of Observations_array.
     * 
     * @param in_files A vector of input Observations files.
     * @param observations An Observations_array to store the binded sub arrays.
     * @param along Which dimension to append counting from 0.
     * @param verbose Verbose level.
     * @return An AnEnIO::errorType;
     */
    static errorType
    combineObservationsArray(
            const std::vector<std::string> & in_files,
            Observations_array & observations, size_t along, int verbose = 2);


    /**
     * Binds a vector of StandardDeviation
     * @param in_files A vector of input standard deviation files.
     * @param sds A StandardDeviation.
     * @param parameters anenPar::Parameters.
     * @param stations anenSta::Stations.
     * @param flts anenTime::FLTs.
     * @param along Which dimension to append counting from 0.
     * @param verbose Verbose level.
     * @return An AnEnIO::errorType;
     */
    static errorType
    combineStandardDeviation(const std::vector<std::string> & in_files,
            StandardDeviation & sds,
            anenPar::Parameters & parameters,
            anenSta::Stations & stations,
            anenTime::FLTs & flts,
            size_t along, int verbose = 2);

    /**
     * Binds a vector of SimilarityMatrices.
     * @param in_files A vector of input standard deviation files.
     * @param sims A SimilarityMatrices.
     * @param stations anenSta::Stations.
     * @param times anenSta::Times.
     * @param flts anenTime::FLTs.
     * @param along Which dimension to append counting from 0.
     * @param verbose Verbose level.
     * @return  An AnEnIO::errorType;
     */
    static errorType
    combineSimilarityMatrices(const std::vector<std::string> & in_files,
            SimilarityMatrices & sims,
            anenSta::Stations & stations,
            anenTime::Times & times,
            anenTime::FLTs & flts,
            size_t along, int verbose = 2);

    /**
     * Binds a vector of Analogs. 
     * @param in_files A vector of input standard deviation files.
     * @param analogs A Analogs.
     * @param stations anenSta::Stations.
     * @param times anenSta::Times.
     * @param flts anenTime::FLTs.
     * @param along Which dimension to append, counting from 0. 
     * @param verbose Verbose level.
     * @return An AnEnIO::errorType.
     */
    static errorType
    combineAnalogs(const std::vector<std::string> & in_files,
            Analogs & analogs,
            anenSta::Stations & stations,
            anenTime::Times & times,
            anenTime::FLTs & flts,
            size_t along, int verbose = 2);

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
     * - Analogs
     * - Similarity
     * - Matrix
     * - StandardDeviation
     */
    std::string file_type_;

    /**
     * Whether to write to an existing file.
     */
    bool add_ = false;

    /**
     * Specifies the verbose level.
     * - 0: Quiet.
     * - 1: Errors.
     * - 2: The above plus Warnings.
     * - 3: The above plus program progress information.
     * - 4: The above plus check information.
     * - 5: The above plus session information.
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

    /**
     * Writes only the similarity matrices. This is a private function because
     * writing a Similarity file requires more variables than just the 
     * SimilarityMatrices.
     * 
     * @param sims A SimilarityMatrices to be written.
     * @return AnEnIO::erroType.
     */
    errorType writeSimilarityMatricesOnly_(
            const SimilarityMatrices & sims) const;

    /**
     * Writes only the Analogs. This is a private function because writing an
     * Analog file requires more variables than just Analogs.
     * 
     * @param analogs Analogs to be written.
     * @return AnEnIO::erroType.
     */
    errorType writeAnalogsOnly_(const Analogs & analogs) const;

    /**
     * Writes only the standard deviation. This is private function because
     * writing a standard deviation file requires more than just the
     * StandardDeviation. 
     * @param sds StandardDeviation to be written.
     * @return AnEnIO::errorType.
     */
    errorType writeStandardDeviationOnly_(const StandardDeviation & sds) const;

    /**
     * Reads observation array data. Observations data should be
     * allocated by Observations_array::updateDataDims.
     * @param observations
     * @return An AnEnIO::errorType.
     */
    errorType readObservationsArrayData_(Observations_array & observations) const;

    /**
     * Reads observation array data. Observations data should be
     * allocated by Observations_array::updateDataDims.
     * @param observations The Observations_array object to store data.
     * @param start A vector of indices of the start of the data to read.
     * @param count A vector of numbers of data to read for each dimension.
     * @param stride A vector of numbers of the length of the stride for
     * each dimension.
     * @return An AnEnIO::errorType.
     */
    errorType readObservationsArrayData_(Observations_array & observations,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1}) const;

    /**
     * Reads variables as a atomic vector.
     * 
     * @param var_name The variable name.
     * @param results An atmoic container.
     * @return AnEnIO::errorType.
     */
    template<typename T>
    errorType
    read_vector_(std::string var_name, std::vector<T> & results) const;

    template<typename T>
    errorType read_vector_(std::string var_name, std::vector<T> & results,
            std::vector<size_t> start, std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1}) const;

    template<typename T>
    errorType read_vector_(std::string var_name, std::vector<T> & results,
            size_t start, size_t count, ptrdiff_t stride = 1) const;

};

#include "AnEnIO.tpp"
#endif /* ANENIO_H */
