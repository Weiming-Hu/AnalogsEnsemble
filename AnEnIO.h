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
#include <exception>

#include "Times.h"
#include "Stations.h"
#include "Observations.h"
#include "Forecasts.h"
#include "colorTexts.h"

class AnEnIO {
public:
    AnEnIO() = delete;
    AnEnIO(const AnEnIO& orig) = delete;

    AnEnIO(std::string file_path);
    AnEnIO(std::string file_path, std::string file_type);
    AnEnIO(std::string file_path, std::string file_type, int verbose);
    AnEnIO(std::string file_path, std::string file_type, int verbose,
            std::vector<std::string> required_variables,
            std::vector<std::string> optional_variables);

    virtual ~AnEnIO();

    /**
     * Specifies the return error type of a function. Use 
     * AnEnIO::handleError to handle the returned errorType variable.
     */
    enum errorType {
        SUCCESS = 0,
        UNKNOWN_ERROR = -1,
        UNKOWN_FILE_TYPE = -2,

        FILE_NOT_FOUND = -10,
        REQUIRED_VARIABLE_MISSING = -11,
        OPTIONAL_VARIABLE_MISSING = -12,
        DIMENSION_MISSING = -13,
        WRONG_VARIABLE_SHAPE = -14,
        WRONG_VARIABLE_TYPE = -15,
        ELEMENT_NOT_UNIQUE = -16,

        ERROR_SETTING_VALUES = -50,

        INSUFFICIENT_MEMORY = -100
    };

    /**
     * Checks whether the NetCDF file exists.
     * 
     * @return AnEnIO::errorType.
     */
    errorType checkFile() const;

    /**
     * Checks whether the NetCDF file conforms with the file type.
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
    errorType checkDim(string dim_name) const;

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

    errorType readData(Observations & observations);
    errorType readData(Forecasts & forecasts);
    errorType readFLTs(FLTs & flts);
    errorType readParameters(Parameters & parameters);
    errorType readStations(Stations & stations);
    errorType readTimes(Times & times);
    errorType readDimLength(std::string dim_name, std::size_t & len);

    /**
     * Handles the errorType variable.
     * 
     * @param indicator An errorType item.
     */
    void handleError(const errorType & indicator) const;

    std::string getFilePath() const;
    std::string getFileType() const;
    std::vector<std::string> getOptionalVariables() const;
    std::vector<std::string> getRequiredVariables() const;
    std::vector<std::string> getRequiredDimensions() const;
    int getVerbose() const;

    void setFilePath(std::string file_path_);
    void setFileType(std::string fileType_);
    void setOptionalVariables(std::vector<std::string> optional_variables);
    void setRequiredVariables(std::vector<std::string> required_variables);
    void setRequiredDimensions(std::vector<std::string> required_dimensions);
    void setVerbose(int verbose_);

    void dumpVariable(std::string var_name,
            std::size_t start = 0,
            std::size_t count = 0) const;

protected:

    /**
     * Specifies the full path to the file.
     */
    std::string file_path_;

    /**
     * Specifies the type of the file. The Value comes from AnEnIO::fileType.
     */
    std::string file_type_;

    /**
     * Specifies the verbose level.
     * - 0: No standard output.
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
     * @param vectors A string container.
     * @return AnEnIO::errorType.
     */
    errorType read_string_vector_(std::string var_name,
            std::vector<std::string> & vectors) const;

    /************************************************************************
     *                      Template Functions                              *
     ************************************************************************/

    /**
     * Reads variables as a atomic vector.
     * 
     * @param var_name The variable name.
     * @param vector An atmoic container.
     * @return AnEnIO::errorType.
     */
    template<typename T>
    errorType
    read_vector_(std::string var_name, std::vector<T> & vector) const {

        using namespace netCDF;
        using namespace std;

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
            if (verbose_ >= 1) cout << BOLDRED <<
                    "Error: Insufficient memory when reading variable ("
                    << var_name << ")!" << RESET << endl;
            return (INSUFFICIENT_MEMORY);
        }

        var.getVar(p_vals);
        vector.assign(p_vals, p_vals + len);

        nc.close();
        delete [] p_vals;
        return (SUCCESS);
    };

};

#endif /* ANENIO_H */