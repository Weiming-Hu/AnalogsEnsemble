/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Stations.h
 * Author: guido
 *
 * Created on April 17, 2018, 10:41 PM
 */

#include <vector>
#include <iostream>

#ifndef STATIONS_H
#define STATIONS_H




class Station {   
    
public:
    Station(); 
    Station(int, double, double);

    virtual ~Station(); 
    
    void SetID(int);
    void SetLatitude(double);
    void SetLongitude(double);
   
    int GetID() const;
    double GetLatitude() const;
    double GetLongitude() const;

    // Print stream
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Station const &);

private:
    int ID_;
    double longitude_;
    double latitude_;
    
};





class Stations : public std::vector <Station> {
public:
    Stations();
    virtual ~Stations();
    
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Stations const &);

private:

};



#endif /* STATIONS_H */

