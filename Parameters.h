/* 
 * File:   Parameters.h
 * Author: guido
 *
 * Created on April 18, 2018, 12:18 AM
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <vector>
#include <iostream>
#include <string>

class Parameter {   
    
public:
    Parameter(); 
    Parameter(std::string, double, bool);
 
    virtual ~Parameter(); 
  
    void setName(std::string);
    void setWeight(double);
    void setCircular(bool);
   
    std::string getName() const;
    double getWeight() const;
    bool getCircular() const;

    // Print stream
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Parameter const &);

private:
    std::string name_;
    double weight_;
    bool circular_;
    
};





class Parameters : public std::vector <Parameter> {
public:
    Parameters();
    virtual ~Parameters();
    
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Parameters const &);

private:

};

#endif /* PARAMETERS_H */

