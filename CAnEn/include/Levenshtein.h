/* 
 * File: Levenshtein.h
 * Author: Weiming Hu
 *
 * Created on Aug 3, 2017, 1:44 PM
 *
 * This file contains the function to compute the Levenshtein distance
 * of two string. The function can be used to guess the intended argument 
 * of an unknown one.
 *
 * The implementation is referenced from Github
 * https://github.com/git/git/blob/master/levenshtein.h
 *
 * Thanks to the pointer of Vlad Lazarenko
 * http://lazarenko.me/smart-getopt/
 */

#ifndef LEVENSHTEIN_H
#define LEVENSHTEIN_H

#include <string>

size_t levenshtein(const std::string & str1, const std::string & str2,
        size_t w, size_t s, size_t a, size_t d);

#endif
