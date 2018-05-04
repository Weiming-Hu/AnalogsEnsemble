/* 
 * File:   Time.h
 * Author: guido
 *
 * Created on April 18, 2018, 12:33 AM
 */

#ifndef TIME_H
#define TIME_H

#include <vector>
#include <string>
#include <iostream>


class Times : public std::vector <double> {
public:
    Times();
    virtual ~Times();
    
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Times const &);
    
private:

    std::string unit_;
    std::string origin_;
    
};


class FLTs : public std::vector <double> {
  public:
    FLTs();
    virtual ~FLTs();
    
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, FLTs const &);
private:
    std::string unit_;
};

#endif /* TIME_H */

