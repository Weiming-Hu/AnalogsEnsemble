/* 
 * File:   colorTexts.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 28, 2018, 3:01 PM
 */

#ifndef COLORTEXTS_H
#define COLORTEXTS_H

// color setting
#if defined _WIN32
#define RESET   ""
#define BOLDRED     ""
#define BOLDGREEN   ""
#define RED     ""
#define GREEN   ""
#else
#define RESET       "\033[0m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#endif

#endif /* COLORTEXTS_H */

