/* 
 * File:   AnEnErr.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 6, 2019, 9:45 PM
 */

#include "AnEnErr.h"

namespace {
    
    struct AnEnErrCategory : std::error_category {
        const char* name() const noexcept override;
        std::string message(int err) const override;
    };
    
    const char* AnEnErrCategory::name() const noexcept {
        return "AnEn error";
    }
    
    std::string AnEnErrCategory::message(int err) const {
        switch (static_cast<AnEnErr>(err)) {
            case AnEnErr::UnknownMethodSim:
                return "Unknown method for computing the similarity.";
            case AnEnErr::MissingValue:
                return "Missing value.";
            case AnEnErr::WrongShape:
                return "Wrong or non-matching data shape.";
            case AnEnErr::WrongMethod:
                return "Wrong method is invoked for this file type.";
            case AnEnErr::OutOfRange:
                return "Index out of range.";
            default:
                return "Unrecognized AnEn error detected.";
        }
    }
    
    const AnEnErrCategory theAnEnErrCategory {};
}

std::error_code make_error_code(const AnEnErr & err) {
    return {static_cast<int>(err), theAnEnErrCategory};
}