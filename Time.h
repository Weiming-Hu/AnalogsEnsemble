/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

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


class Time : public std::vector <int> {
public:
    Time();
    virtual ~Time();
    
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Time const &);
    
private:

    std::string unit_;
    std::string origin_;
    
};


class FLT : public std::vector <int> {
  public:
    FLT();
    virtual ~FLT();
    
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, FLT const &);
private:
    std::string unit_;
};

#endif /* TIME_H */

