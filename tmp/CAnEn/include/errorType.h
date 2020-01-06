/* 
 * File:   errorType.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 7, 2019, 12:25 PM
 */

#ifndef ERRORTYPE_H
#define ERRORTYPE_H

/**
 * Error codes.
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
    /// -9
    UNKNOWN_FILE_EXTENSION = -9,

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

    /// -61
    UNKNOWN_METHOD = -61,
    /// -62
    MISSING_VALUE = -62,
    
    /// -70
    WRONG_SHAPE = -70,
    /// -71
    WRONG_METHOD = -71,
    
    /// -80
    OUT_OF_RANGE = -80,

    /// -100
    INSUFFICIENT_MEMORY = -100
};

/**
 * Handles the errorType.
 * 
 * @param indicator An errorType.
 */
void handleError(const errorType & indicator);

#endif /* ERRORTYPE_H */

