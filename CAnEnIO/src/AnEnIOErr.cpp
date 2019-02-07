/* 
 * File:   AnEnIOErr.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 6, 2019, 10:24 PM
 */

#include "AnEnIOErr.h"

namespace {
    struct AnEnIOErrCategory : std::error_category {
        const char* name() const noexcept override;
        std::string message(int err) const override;
    };
    
    const char* AnEnIOErrCategory::name() const noexcept {
        return "AnEnIO error";
    }
    
    std::string AnEnIOErrCategory::message(int err) const {
        switch (static_cast<AnEnIOErr>(err)) {
            case AnEnIOErr::UnknownFileMode:
                return "Unknown file mode.";
            case AnEnIOErr::UnknownFileType:
                return "Unknown file type.";
            case AnEnIOErr::WrongFileMode:
                return "Wrong file mode.";
            case AnEnIOErr::WrongFileType:
                return "Wrong file type.";
            case AnEnIOErr::FileExists:
                return "File exists.";
            case AnEnIOErr::VariableExists:
                return "Variable exists.";
            case AnEnIOErr::DimensionExists:
                return "Dimension exists.";
            case AnEnIOErr::MissingFile:
                return "File cannot be found.";
            case AnEnIOErr::MissingDimension:
                return "Dimension cannot be found.";
            case AnEnIOErr::MissingRequiredVariable:
                return "Required variable cannot be found.";
            case AnEnIOErr::MissingOptionalVariable:
                return "Optional variable cannot be found.";
            case AnEnIOErr::WrongIndexLength:
                return "Non-matching length of input Indices.";
            case AnEnIOErr::WrongVariableType:
                return "Non-matching variable data type.";
            case AnEnIOErr::WrongVariableShape:
                return "Non-matching variable shape";
            case AnEnIOErr::WrongDimensionAppend:
                return "The specified dimension to append is wrong.";
            case AnEnIOErr::NotAValue:
                return "Not a value.";
            case AnEnIOErr::FileIOError:
                return "File I/O error.";
            case AnEnIOErr::DuplicateElement:
                return "Element is not unique.";
            case AnEnIOErr::InsufficientMemory:
                return "Insufficient memory.";
            case AnEnIOErr::MethodNotImplemented:
                return "Method not implemented.";
            default:
                return "Unrecognized AnEnIO error detected.";
        }
    }
    
    const AnEnIOErrCategory theAnEnIOErrCategory  {};
}

std::error_code make_error_code(const AnEnIOErr & err) {
    return {static_cast<int>(err), theAnEnIOErrCategory};
}
