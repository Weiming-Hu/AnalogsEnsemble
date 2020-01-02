/* 
 * File:   AnEnWriteNcdf.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on January 1, 2020, 6:33 PM
 */

#ifndef ANENWRITENCDF_H
#define ANENWRITENCDF_H

#include "AnEnWrite.h"

class AnEnWriteNcdf : public AnEnWrite {
public:
    AnEnWriteNcdf();
    AnEnWriteNcdf(const AnEnWriteNcdf& orig);
    virtual ~AnEnWriteNcdf();
    
    void writeAnalogs(const std::string & file_path,
            const Analogs & analogs) const override;
};

#endif /* ANENWRITENCDF_H */

