/* 
 * File:   colorTexts.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 28, 2018, 3:01 PM
 */

#ifndef COLORTEXTS_H
#define COLORTEXTS_H

#include <string>

// color setting
#if defined _WIN32
static const std::string RESET = "";
static const std::string BOLDRED = "";
static const std::string BOLDGREEN = "";
static const std::string RED = "";
static const std::string GREEN = "";
#else
static const std::string RESET = "\033[0m";
static const std::string BOLDRED = "\033[1m\033[31m";
static const std::string BOLDGREEN = "\033[1m\033[32m";
static const std::string RED = "\033[31m";
static const std::string GREEN = "\033[32m";
#endif

#endif /* COLORTEXTS_H */

