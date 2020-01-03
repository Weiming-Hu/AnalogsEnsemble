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
#include <algorithm>

#include "Times.h"
#include "Analogs.h"
#include "Stations.h"
#include "Forecasts.h"
#include "errorType.h"
#include "Parameters.h"
#include "colorTexts.h"
#include "Observations.h"
#include "StandardDeviation.h"
#include "SimilarityMatrices.h"
#include "boost/numeric/ublas/matrix.hpp"

#if defined(_OPENMP)
#include <omp.h>
#endif

#if defined(_ENABLE_MPI)
#include <mpi.h>
#endif

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
    /*
     * The name prefix for member dimensions in file type Analogs
     */
    const static std::string _MEMBER_DIM_PREFIX;
    
    /*
     * The name prefix for member variables in file type Analogs
     */
    const static std::string _MEMBER_VAR_PREFIX;
    
    /*
     * The name prefix for search dimensions in file type SimilarityMatrices
     */
    const static std::string _SEARCH_DIM_PREFIX;
    
    /*
     * The name prefix for search variables in file type SimilarityMatrices
     */
    const static std::string _SEARCH_VAR_PREFIX;
    
    /*
     * The length upper limit of data for serial file I/O. If data have a
     * length longer than this, the I/O will be parallelized using MPI.
     */
    const static size_t _SERIAL_LENGTH_LIMIT;
    
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
    
#if defined(_ENABLE_MPI)
    /*
     * This function handles the MPI Init. It makes sure that MPI_Init only gets
     * called once during the lifetime of the program.
     */
    static void handle_MPI_Init();
    
    /*
     * This function handles the MPI Finalize. It makes sure that MPI_Finalize 
     * only gets called once during the lifetime of the program.
     */
    static void handle_MPI_Finalize();
#endif

    /**
     * Checks object mode for input/output.
     * 
     * @return errorType
     */
    errorType checkMode() const;

    /**
     * Checks file path and file extension.
     * 
     * @return errorType.
     */
    errorType checkFilePath() const;

    /**
     * Checks whether the NetCDF file conforms with the file type.
     * 
     * The supported file type are
     * - Observations
     * - Forecasts
     * 
     * @return errorType.
     */
    errorType checkFileType() const;

    /**
     * Checks whether the variable exists.
     * @param var_name The variable name.
     * @param optional Whether the variable is optional.
     * @return errorType.
     */
    errorType checkVariable(std::string var_name, bool optional) const;

    /**
     * Checks whether the dimension exists.
     * 
     * @param dim_name The dimension name.
     * @return errorType.
     */
    errorType checkDim(std::string dim_name) const;

    /**
     * Checks the required and optional variables.
     * @return errorType.
     */
    errorType checkVariables() const;

    /**
     * Checks the required dimensions.
     * @return errorType.
     */
    errorType checkDimensions() const;

    /**
     * Reads observation file into an Observations object.
     * 
     * @param observations The Observations_array object to store data.
     * @return An errorType.
     */
    errorType readObservations(ObservationsArray & observations) const;

    /**
     * Reads a subset of an observation file into an Observations
     * object.
     * 
     * @param observations The Observations_array object to store data.
     * @param start A vector of indices of the start of the data to read.
     * @param count A vector of numbers of data to read for each dimension.
     * @param stride A vector of numbers of the length of the stride for
     * each dimension.
     * @return An errorType.
     */
    errorType readObservations(ObservationsArray & observations,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1}) const;

    /**
     * Reads forecast file into an Forecasts object.
     * 
     * @param forecasts The Forecasts_array object to store data.
     * @return An errorType.
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
     * @return An errorType.
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
     * @return An errorType.
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
     * @return An errorType.
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
     * @return An errorType.
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
     * @return An errorType.
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
     * @return An errorType.
     */
    errorType readDimLength(std::string dim_name, std::size_t & len) const;

    /**
     * Write forecasts.
     * @param forecasts Forecasts to write.
     * @return An errorType.
     */
    errorType writeForecasts(const Forecasts & forecasts) const;

    /**
     * Write observations.
     * @param observations Observations to write.
     * @return An errorType.
     */
    errorType writeObservations(const Observations & observations) const;

    /**
     * Write Forecast Lead Times anenTime::FLTs. A dimension (num_flts)
     * will be created; Variables for FLTs will be created.
     * @param flts anenTime::FLTs to write.
     * @param unlimited Whether this dimension is unlimited.
     * @return An errorType.
     */
    errorType writeFLTs(const anenTime::FLTs & flts, bool unlimited) const;

    /**
     * Write anenPar::Parameters. A dimension (num_parameters) will be created;
     * Variables for "ParameterNames", "ParameterWeights", "ParameterCirculars"
     * will be created.
     * @param parameters anenPar::Parameters to write.
     * @param unlimited Whether this dimension is unlimited.
     * @return An errorType.
     */
    errorType writeParameters(
            const anenPar::Parameters & parameters, bool unlimited) const;

    /**
     * Write anenSta::Stations. A dimension (num_stations) will be created;
     * Variables for name of stations, Xs, and Ys will be created.
     * @param stations anenSta::Stations to write.
     * @param unlimited Whether this dimension is unlimited.
     * @return An errorType.
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
     * @return An errorType.
     */
    errorType writeTimes(const anenTime::Times & times, bool unlimited,
            const std::string & dim_name = "num_times",
            const std::string & var_name = "Times") const;

    /**
     * Read SimilarityMatrices.
     * @param sims SimilarityMatrices to store data.
     * @return An errorType.
     */
    errorType readSimilarityMatrices(SimilarityMatrices & sims);
    errorType readSimilarityMatrices(SimilarityMatrices & sims,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1, 1, 1});

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
     * @return An errorType.
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
     * @return An errorType.
     */
    errorType readAnalogs(Analogs & analogs);
    errorType readAnalogs(Analogs & analogs,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1, 1, 1});

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
     * @return An errorType.
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
     * @return An errorType. 
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
     * @return errorType.
     */
    errorType writeStandardDeviation(
            const StandardDeviation & sds,
            const anenPar::Parameters & parameters,
            const anenSta::Stations & stations,
            const anenTime::FLTs & flts) const;

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

    void setVerbose(int verbose);
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
     * @return An errorType.
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
     * @return An errorType.
     */
    template <typename T>
    errorType
    writeTextMatrix(const boost::numeric::ublas::matrix<T> & mapping) const;

    static errorType
    combineParameters(
            const std::vector<std::string> & in_files,
            const std::string & file_type,
            anenPar::Parameters & parameters, int verbose,
            const std::vector<size_t> & start = {},
            const std::vector<size_t> & count = {}, 
            bool allow_duplicates = false);

    static errorType
    combineStations(
            const std::vector<std::string> & in_files,
            const std::string & file_type,
            anenSta::Stations & stations, int verbose,
            const std::string & dim_name_prefix = "",
            const std::string & var_name_prefix = "",
            const std::vector<size_t> & start = {},
            const std::vector<size_t> & count = {}, 
            bool allow_duplicates = false);


    static errorType
    combineTimes(
            const std::vector<std::string> & in_files,
            const std::string & file_type,
            anenTime::Times & times, int verbose,
            const std::string & var_name = "Times",
            const std::vector<size_t> & start = {},
            const std::vector<size_t> & count = {}, 
            bool allow_duplicates = false);

    static errorType
    combineFLTs(
            const std::vector<std::string> & in_files,
            const std::string & file_type,
            anenTime::FLTs & flts, int verbose,
            const std::vector<size_t> & start = {},
            const std::vector<size_t> & count = {}, 
            bool allow_duplicates = false);

    /**
     * Binds a vector of Forecasts_array.
     * 
     * @param in_files A vector of input Forecasts files.
     * @param forecasts A Forecasts_array to store the binded sub arrays.
     * @param along Which dimension to append. 0 for parameters, 1 for 
     * stations, 2 for times, and 3 for forecast lead times.
     * @param verbose Verbose level.
     * @param starts The start index for each dimension.
     * @param counts The count number for each dimension.
     * @return An errorType;
     */
    static errorType
    combineForecastsArray(
            const std::vector<std::string> & in_files,
            Forecasts_array & forecasts, size_t along, int verbose = 2,
            const std::vector<size_t> & starts = {},
            const std::vector<size_t> & counts = {});

    /**
     * Binds a vector of Observations_array.
     * 
     * @param in_files A vector of input Observations files.
     * @param observations An Observations_array to store the binded sub arrays.
     * @param along Which dimension to append to. 0 for parameters, 1 for 
     * stations, and 2 for times.
     * @param verbose Verbose level.
     * @return An errorType;
     */
    static errorType
    combineObservationsArray(
            const std::vector<std::string> & in_files,
            ObservationsArray & observations, size_t along, int verbose = 2,
            const std::vector<size_t> & starts = {},
            const std::vector<size_t> & counts = {});


    /**
     * Binds a vector of StandardDeviation
     * @param in_files A vector of input standard deviation files.
     * @param sds A StandardDeviation.
     * @param parameters anenPar::Parameters.
     * @param stations anenSta::Stations.
     * @param flts anenTime::FLTs.
     * @param along Which dimension to append to. 0 for parameters, 1 for 
     * stations, and 2 for forecast lead times.
     * @param verbose Verbose level.
     * @return An errorType;
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
     * @param along Which dimension to append to. 0 for stations, 1 for
     * times, 2 for forecast lead times, 3 for entries.
     * @param verbose Verbose level.
     * @return  An errorType;
     */
    static errorType
    combineSimilarityMatrices(const std::vector<std::string> & in_files,
            SimilarityMatrices & sims,
            anenSta::Stations & stations,
            anenTime::Times & times,
            anenTime::FLTs & flts,
            anenSta::Stations & search_stations,
            anenTime::Times & search_times,
            size_t along, int verbose = 2,
            const std::vector<size_t> & starts = {},
            const std::vector<size_t> & counts = {});

    /**
     * Binds a vector of Analogs. 
     * @param in_files A vector of input standard deviation files.
     * @param analogs A Analogs.
     * @param stations anenSta::Stations.
     * @param times anenSta::Times.
     * @param flts anenTime::FLTs.
     * @param along Which dimension to append to. 0 for stations, 1 for
     * times, 2 for forecast lead times, and 3 for ensemble members.
     * @param verbose Verbose level.
     * @return An errorType.
     */
    static errorType
    combineAnalogs(const std::vector<std::string> & in_files,
            Analogs & analogs,
            anenSta::Stations & stations,
            anenTime::Times & times,
            anenTime::FLTs & flts,
            anenSta::Stations & member_stations,
            anenTime::Times & member_times,
            size_t along, int verbose = 2,
            const std::vector<size_t> & starts = {},
            const std::vector<size_t> & counts = {});

protected:

#if defined(_ENABLE_MPI)
    static int _times_of_MPI_init;
#endif
    
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
     * Whether the file is a NetCDF 4 format.
     */
    bool NC4_ = false;

    /**
     * Specifies the verbose level.
     * - 0: Quiet.
     * - 1: Errors.
     * - 2: The above plus Warnings.
     * - 3: The above plus program progress information.
     * - 4: The above plus check information.
     * - 5: The above plus session information.
     *
     * Please note that the verbose level of thread level messages
     * is (verbose - 2). For example, if the main program verbose
     * level is 5, program progress information from threads
     * , which is for verbose 3, will be included.
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
     * @return errorType.
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
     * @return errorType.
     */
    errorType writeStandardDeviationOnly_(const StandardDeviation & sds) const;

    /**
     * Reads forecasts array data. Forecasts data should be
     * allocated by Forecasts_array::updateDataDims.
     * @param forecasts
     * @return An errorType.
     */
    errorType readForecastsArrayData_(Forecasts_array & forecasts) const;

    /**
     * Reads forecasts array data. Forecasts data should be
     * allocated by Forecasts_array::updateDataDims.
     * @param forecasts The Forecasts object to store data.
     * @param start A vector of indices of the start of the data to read.
     * @param count A vector of numbers of data to read for each dimension.
     * @param stride A vector of numbers of the length of the stride for
     * each dimension.
     * @return An errorType.
     */
    errorType readForecastsArrayData_(Forecasts_array & forecasts,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1}) const;

    /**
     * Reads observation array data. Observations data should be
     * allocated by Observations_array::updateDataDims.
     * @param observations
     * @return An errorType.
     */
    errorType readObservationsArrayData_(ObservationsArray & observations) const;

    /**
     * Reads observation array data. Observations data should be
     * allocated by Observations_array::updateDataDims.
     * @param observations The Observations_array object to store data.
     * @param start A vector of indices of the start of the data to read.
     * @param count A vector of numbers of data to read for each dimension.
     * @param stride A vector of numbers of the length of the stride for
     * each dimension.
     * @return An errorType.
     */
    errorType readObservationsArrayData_(ObservationsArray & observations,
            std::vector<size_t> start,
            std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1, 1, 1}) const;
    
    /**
     * Creates parameters and insert individual parameter into the container.
     * This is an optimized implementation of the common insert function provided
     * by the container. It will parallelize the function if OpenMP is supported
     * and there are enough values to be written.
     * 
     * @param parameters anenPar::Parameters container.
     * @param dim_len The number of parameters to be written.
     * @param names The names of parameters.
     * @param circulars The names of circular parameters.
     * @param weights The weights of parameters.
     * @return An errorType;
     */
    errorType insertParameters_(anenPar::Parameters & parameters, size_t dim_len,
        const std::vector<std::string> & names, std::vector<std::string> & circulars, 
        const std::vector<double> & weights) const;
    
    /**
     * Creates stations and insert individual station into the container.
     * This is an optimized implementation of the common insert function provided
     * by the container. It will parallelize the function if OpenMP is supported
     * and there are enough values to be written.
     * 
     * @param stations anenSta::Stations container.
     * @param dim_len The number of stations to be written.
     * @param names, The names of stations.
     * @param xs the x coordinates of stations.
     * @param ys the y coordinates of stations.
     * @return An errorType.
     */
    errorType insertStations_(anenSta::Stations & stations, size_t dim_len,
            const std::vector<std::string> & names, const std::vector<double> & xs,
            const std::vector<double> & ys) const;
    
    /**
     * Reads variables as a atomic vector.
     * 
     * @param var_name The variable name.
     * @param results An atomic container.
     * @return errorType.
     */
    template<typename T>
    errorType
    read_vector_(std::string var_name, std::vector<T> & results) const;

    template<typename T>
    void read_vector_(const netCDF::NcVar & var, T *p_vals,
            const size_t & total) const;

    template<typename T>
    errorType read_vector_(std::string var_name, std::vector<T> & results,
            std::vector<size_t> start, std::vector<size_t> count,
            std::vector<ptrdiff_t> stride = {1}) const;

    template<typename T>
    errorType read_vector_(std::string var_name, std::vector<T> & results,
            size_t start, size_t count, ptrdiff_t stride = 1) const;

    template<typename T>
    void read_vector_(const netCDF::NcVar & var, T *p_vals,
            const std::vector<size_t> & start,
            const std::vector<size_t> & count,
            const std::vector<ptrdiff_t> & stride,
            const size_t & total) const;
    
    template<typename T>
    static errorType
    compareNewDimensions(
            const std::vector<size_t> & same_dimensions,
            T * new_shape,
            T * expected_shape,
            int verbose);


#if defined(_OPENMP)
    /**
     * This function reads values with multi-threads. 
     * 
     * @param var A netCDF::NcVar object. This is used to get the name of the
     * variable and the dimensions of the variable.
     * @param results The pointer to store data.
     * @param start The start indices.
     * @param count The count number.
     */
    template<typename T>
    void OpenMP_read_vector_(const netCDF::NcVar & var, T* & results,
            std::vector<size_t> start, std::vector<size_t> count) const;
#endif

#if defined(_ENABLE_MPI)
    /**
     * The path of the executable mpiAnEnIO. By default, this is the name
     * of the executable. It will be automatically found if it exists in
     * PATH.
     */
    std::string mpiAnEnIO_ = "mpiAnEnIO";

    /**
     * This function returns the MPI_Datatype variable based on the template
     * type name. This is useful for a generic programming purpose.
     * 
     * @return The MPI_Datatype to be passed.
     */
    template<typename T>
    MPI_Datatype get_mpi_type() const;

    /**
     * This function gathers a dynamic type from spawned children.
     * 
     * @param recvbuf Receiver buffer.
     * @param recvcounts Receiver counts.
     * @param displs Receiver displacement from the start.
     * @param root The root rank.
     * @param comm The communicator.
     * @return Return status.
     */
    template<typename T>
    int my_MPI_Gatherv(
            void* recvbuf, const int* recvcounts, const int* displs,
            int root, MPI_Comm comm) const;

    
    /**
     * This function reads values using MPI functions. A group of children will
     * be spawned and coordinated within this function to read the specific 
     * variable. At the end of this function before it returns, these spawned
     * processes will be killed and data will be collected from the children.
     * 
     * IF YOU ARE USING MPI, please call the AnEnIO::handle_MPI_Init once 
     * when the program starts and the AnEnIO::handle_MPI_Finalize once when
     * the program is about to end.
     * 
     * @param var A netCDF::NcVar object. This is used to get the name of the
     * variable and the dimensions of the variable.
     * @param results The pointer to store data.
     * @param start The start indices.
     * @param count The count number.
     */
    template<typename T>
    void MPI_read_vector_(const netCDF::NcVar & var, T* & results,
        const std::vector<size_t> & start, const std::vector<size_t> & count) const;
#endif

};

#include "AnEnIO.tpp"
#endif /* ANENIO_H */
