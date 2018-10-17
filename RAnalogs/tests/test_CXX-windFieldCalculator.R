# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <wuh20@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University

# This script is testing the C++ application windFieldCalculator

library(RAnEn)

# Genearte NetCDF file
unlink('wind-forecasts.nc')
unlink('wind-observations.nc')
source('data-to-NetCDF-wind.R')


##################################################################################
#                                 Test 1: Forecasts file                         #
##################################################################################
rm(list = ls())
load('test-Wind.Rdata')
exe <- '../../output/bin/windFieldCalculator'
file_in <- 'wind-forecasts.nc'
file_out <- 'new-wind-forecasts.nc'
file_type <- 'Forecasts'
U <- 'parameter1'
V <- 'parameter2'
dir_name <- 'myWindDirection'
speed_name <- 'myWindSpeed'
verbose <- 1

command <- paste(exe, '--file_in', file_in,
                 '--file_type', file_type,
                 '--file_out', file_out,
                 '-U', U, '-V', V,
                 '--dir_name', dir_name,
                 '--speed_name', speed_name,
                 '-v', verbose)

if (verbose >= 3) {
  print(command)
}


# Calculation
unlink(file_out)
ret <- try(system(command))

# Validation
if (ret == 0) {
  
  nc <- nc_open(file_out)
  
  pars <- ncvar_get(nc, 'ParameterNames')
  assertthat::assert_that(dir_name %in% pars)
  assertthat::assert_that(speed_name %in% pars)
  
  data <- ncvar_get(nc, 'Data')
  
  dir_C <- as.vector(data[which(pars == dir_name), , , , drop = F])
  dir_R <- computeWindDir(as.vector(data[which(pars == U), , , , drop = F]),
                          as.vector(data[which(pars == V), , , , drop = F]))
  
  if (sum(abs(dir_C - dir_R)) > 1e-8) {
    stop("Forecast test failed in wind direction.")
  }
  
  speed_C <- as.vector(data[which(pars == speed_name), , , , drop = F])
  speed_R <- computeWindSpeed(as.vector(data[which(pars == U), , , , drop = F]),
                              as.vector(data[which(pars == V), , , , drop = F]))
  identical(speed_R, speed_C)
  
  nc_close(nc)
  
} else {
  stop("Error: Wind test forecast failed!")
}


##################################################################################
#                               Test 2: Observation file                         #
##################################################################################
unlink(file_out)
file_out <- 'new-wind-observations.nc'
file_in <- 'wind-observations.nc'
file_type <- 'Observations'
V <- 'parameter1'

command <- paste(exe, '--file_in', file_in,
                 '--file_type', file_type,
                 '--file_out', file_out,
                 '-U', U, '-V', V,
                 '--dir_name', dir_name,
                 '--speed_name', speed_name,
                 '-v', verbose)

if (verbose >= 3) {
  print(command)
}

# Calculation
unlink(file_out)
ret <- try(system(command))

# Validation
if (ret == 0) {
  
  nc <- nc_open(file_out)
  
  pars <- ncvar_get(nc, 'ParameterNames')
  assertthat::assert_that(dir_name %in% pars)
  assertthat::assert_that(speed_name %in% pars)
  
  data <- ncvar_get(nc, 'Data')
  
  dir_C <- as.vector(data[which(pars == dir_name), , , drop = F])
  dir_R <- computeWindDir(as.vector(data[which(pars == U), , , drop = F]),
                          as.vector(data[which(pars == V), , , drop = F]))
  
  identical(speed_R, speed_C)
  
  speed_C <- as.vector(data[which(pars == speed_name), , , drop = F])
  speed_R <- computeWindSpeed(as.vector(data[which(pars == U), , , drop = F]),
                              as.vector(data[which(pars == V), , , drop = F]))
  identical(speed_R, speed_C)
  
  nc_close(nc)
  
} else {
  stop("Error: Wind test forecast failed!")
}

unlink('wind-forecasts.nc')
unlink('wind-observations.nc')
unlink(file_out)

print("You survived the Wind and the Solar tests!")
