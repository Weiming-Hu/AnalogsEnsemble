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
#include <string>
#include <set>

#ifndef STATIONS_H
#define STATIONS_H




class Station final {   
    
public:
    
    Station(std::string, double, double);
    Station( Station const &);
    
    virtual ~Station(); 

    
    Station & operator = ( const Station &);    
    
    bool operator == ( const Station &) const;
    bool operator < ( const Station &) const;    
    
    //void setName( std::string );
    //void setY(double);
    //void setX(double);
   
    int getID() const;
    std::string getName() const;
    double getY() const;
    double getX() const;

    // Print stream
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Station const &);

private:
    Station();  
    void set_ID_();
    
    // This is a unique identifier that is used to keep track of the stations
    unsigned int ID_;
    
    std::string name_;
    double x_;
    double y_;
    
    
    /// static variable for serial number identification 
    static unsigned int _static_ID_;
};





class Stations : public std::set <Station> {
public:
    Stations();
    virtual ~Stations();
    
    Station const & operator [] ( int ) const;    // Return by ID
    Station const & operator [] ( Station const &  ) const;    
    
    std::vector<int> getNearbyStations( Station const & ) const;   
    std::vector<int> getNearbyStations( int ) const;   
    
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Stations const &);

private:

};



#endif /* STATIONS_H */

