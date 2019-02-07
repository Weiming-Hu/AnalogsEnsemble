/* 
 * File:   AnEnErr.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 6, 2019, 9:45 PM
 */

#ifndef ANENERR_H
#define ANENERR_H

# include <system_error>

/**
 * Specifies the return error type of an AnEn library function.
 */
enum class AnEnErr {
    UnknownMethodSim,
    MissingValue,
    WrongShape,
    WrongMethod,
    OutOfRange,
};

namespace std {
    template <>
    struct is_error_code_enum<AnEnErr> : true_type {};
}

std::error_code make_error_code(AnEnErr);

#endif /* ANENERR_H */