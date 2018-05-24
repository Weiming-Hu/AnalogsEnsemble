/* 
 * File:   Time.h
 * Author: guido
 *
 * Created on April 18, 2018, 12:33 AM
 */

#ifndef TIME_H
#define TIME_H

#include <set>
#include <string>
#include <iostream>


class Times : public std::set <double> 
{
public:
    
    /// Default Unit: Seconds, Default origin is null
    Times();
    
    /// Default origin is null
    Times(std::string);
        
    /// Specify both unit and origin 
    Times(std::string, std::string);
    
    virtual ~Times();
    
    // We might want to add a convert function to change units and origin    
    
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Times const &);
    
protected:
    
    std::string unit_;
    std::string origin_;    
};

class FLTs : public Times
{

friend std::ostream& operator<<(std::ostream&, FLTs const &);
};


#endif /* TIME_H */

