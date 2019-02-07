/* 
 * File:   AnEnIOErr.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 6, 2019, 10:24 PM
 */

#ifndef ANENIOERR_H
#define ANENIOERR_H

#include <system_error>

/**
 * Specifies the return error type of an AnEnIO library function.
 */
enum class AnEnIOErr {
    
    UnknownFileMode,
    UnknownFileType,
    
    WrongFileMode,
    WrongFileType,
    
    FileExists,
    VariableExists,
    DimensionExists,
    
    MissingFile,
    MissingDimension,
    MissingRequiredVariable,
    MissingOptionalVariable,
    
    WrongIndexLength,
    WrongVariableType,
    WrongVariableShape,
    WrongDimensionAppend,
    
    NotAValue,
    FileIOError,
    DuplicateElement,
    InsufficientMemory,
    MethodNotImplemented,

};

namespace std {
    template <>
    struct is_error_code_enum<AnEnIOErr> : true_type {};
}

std::error_code make_error_code(AnEnIOErr);

#endif /* ANENIOERR_H */

