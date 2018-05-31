/* 
 * File:   Time.h
 * Author: guido
 *
 * Created on April 18, 2018, 12:33 AM
 */

#ifndef TIME_H
#define TIME_H

#include <unordered_set>
#include <string>
#include <iostream>

class Times : public std::set<double> {
public:
    Times();
    Times(std::string unit);
    Times(std::string unit, std::string origin);

    virtual ~Times();

    void print(std::ostream & os) const;
    friend std::ostream& operator<<(std::ostream& os, Times const & obj);

protected:
    std::string unit_ = "seconds";
    std::string origin_;
};

class FLTs : public Times {
    void print(std::ostream & os) const;
    friend std::ostream& operator<<(std::ostream& os, FLTs const & obj);
};


#endif /* TIME_H */

