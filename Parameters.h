/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

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
  
    void SetID(std::string);
    void SetWeight(double);
    void SetCircular(bool);
   
    std::string GetID() const;
    double GetWeight() const;
    bool GetCircular() const;

    // Print stream
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Parameter const &);

private:
    std::string ID_;
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

