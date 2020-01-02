/* 
 * File:   AnEnWrite.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on January 1, 2020, 6:29 PM
 */

#ifndef ANENWRITE_H
#define ANENWRITE_H

#include "Analogs.h"

/**
 * \class AnEnWrite
 * 
 * \brief AnEnWrite is an abstract class that defines the interface for the
 * writing functionality required for AnEn.
 */
class AnEnWrite {
public:
    AnEnWrite();
    AnEnWrite(const AnEnWrite& orig);
    virtual ~AnEnWrite();

    virtual void writeAnalogs(
            const std::string & file_path,
            const Analogs & analogs) const = 0;
};

#endif /* ANENWRITE_H */