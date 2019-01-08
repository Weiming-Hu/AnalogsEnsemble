# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <weiming@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

# This script generates data for profiling AnEn programs to avoid saving data on the server.
library(ncdf4)
rm(list = ls())

#########################################################################################
#                                 Generate Forecasts                                    #
#########################################################################################
# Dimensions:
# - num_parameters
# - num_stations
# - num_times
# - num_flts
# - num_chars
#
# Variables:
# - Data
# - FLTs
# - Times
# - StationNames
# - ParameterNames
# - ParameterCirculars
#

cat("Generating test forecast data ...\n")
file_forecasts <- 'test-forecasts.nc'

num_parameters <- 10
num_stations <- 800
num_times <- 30
num_flts <- 12
num_chars <- 10

fc <- runif(num_parameters * num_stations * num_times * num_flts,
            num_parameters * 10, num_parameters * 100)

value_flts <- ((1:num_flts) - 1) * 60 * 60
value_times <- ((1:num_times) - 1) * 24 * 60 * 60
value_station_names <- paste("station", 1:num_stations, sep = '')
value_parameter_names <- paste("parameter", 1:num_parameters, sep = '')
value_circulars <- rep('', num_parameters)

dim_parameters <- ncdim_def("num_parameters", "", 1:num_parameters, create_dimvar = F)
dim_stations <- ncdim_def("num_stations", "", 1:num_stations, create_dimvar = F)
dim_times <- ncdim_def("num_times", "", 1:num_times, create_dimvar = F)
dim_flts <- ncdim_def("num_flts", "", 1:num_flts, create_dimvar = F)
dim_chars <- ncdim_def('num_chars', "", 1:num_chars, create_dimvar = F)

var_flts <- ncvar_def("FLTs", "second", list(dim_flts))
var_times <- ncvar_def("Times", "second", list(dim_times))
var_station_names <- ncvar_def("StationNames", "char", list(dim_chars, dim_stations), prec = 'char')
var_circulars <- ncvar_def("ParameterCirculars", "char", list(dim_chars, dim_parameters), prec = 'char')
var_parameter_names <- ncvar_def("ParameterNames", "char", list(dim_chars, dim_parameters), prec = 'char')
var_data <- ncvar_def("Data", "", list(dim_parameters, dim_stations, dim_times, dim_flts), missval = NA, prec = 'double')

unlink(file_forecasts)
ncout <- nc_create(file_forecasts, list(var_data, var_flts, var_times, var_station_names, var_parameter_names, var_circulars), force_v4 = T)

ncvar_put(ncout, var_flts, value_flts)
ncvar_put(ncout, var_times, value_times)
ncvar_put(ncout, var_station_names, value_station_names)
ncvar_put(ncout, var_parameter_names, value_parameter_names)
ncvar_put(ncout, var_circulars, value_circulars)
ncvar_put(ncout, var_data, fc)
nc_close(ncout)

#########################################################################################

cat("Generating search forecast data ...\n")
file_forecasts <- 'search-forecasts.nc'

num_times <- 365 * 3
num_chars <- 10

fc <- runif(num_parameters * num_stations * num_times * num_flts,
            num_parameters * 10, num_parameters * 100)

value_flts <- ((1:num_flts) - 1) * 60 * 60
value_times <- ((1:num_times) - 1) * 24 * 60 * 60
value_station_names <- paste("station", 1:num_stations, sep = '')
value_parameter_names <- paste("parameter", 1:num_parameters, sep = '')
value_circulars <- rep('', num_parameters)

dim_parameters <- ncdim_def("num_parameters", "", 1:num_parameters, create_dimvar = F)
dim_stations <- ncdim_def("num_stations", "", 1:num_stations, create_dimvar = F)
dim_times <- ncdim_def("num_times", "", 1:num_times, create_dimvar = F)
dim_flts <- ncdim_def("num_flts", "", 1:num_flts, create_dimvar = F)
dim_chars <- ncdim_def('num_chars', "", 1:num_chars, create_dimvar = F)

var_flts <- ncvar_def("FLTs", "second", list(dim_flts))
var_times <- ncvar_def("Times", "second", list(dim_times))
var_station_names <- ncvar_def("StationNames", "char", list(dim_chars, dim_stations), prec = 'char')
var_circulars <- ncvar_def("ParameterCirculars", "char", list(dim_chars, dim_parameters), prec = 'char')
var_parameter_names <- ncvar_def("ParameterNames", "char", list(dim_chars, dim_parameters), prec = 'char')
var_data <- ncvar_def("Data", "", list(dim_parameters, dim_stations, dim_times, dim_flts), missval = NA, prec = 'double')

unlink(file_forecasts)
ncout <- nc_create(file_forecasts, list(var_flts, var_data, var_times, var_station_names, var_parameter_names, var_circulars), force_v4 = T)

ncvar_put(ncout, var_flts, value_flts)
ncvar_put(ncout, var_times, value_times)
ncvar_put(ncout, var_station_names, value_station_names)
ncvar_put(ncout, var_parameter_names, value_parameter_names)
ncvar_put(ncout, var_circulars, value_circulars)
ncvar_put(ncout, var_data, fc)
nc_close(ncout)

#########################################################################################
#                                 Generate Observations                                 #
#########################################################################################
# Dimensions:
# - num_parameters
# - num_stations
# - num_times
# - num_chars
#
# Variables:
# - Data
# - Times
# - StationNames
# - ParameterNames
# - ParameterCirculars
#
cat("Generating observation data ...\n")
file_observations <- 'observations.nc'

num_parameters <- 1
num_times <- num_times * num_flts

obs <- runif(num_parameters * num_stations * num_times,
             num_parameters * 10, num_parameters * 100)

value_times <- rep(value_times, each = length(value_flts)) + value_flts
value_parameter_names <- paste("parameter", 1:num_parameters, sep = '')
value_circulars <- rep('', num_parameters)

dim_parameters <- ncdim_def("num_parameters", "", 1:num_parameters, create_dimvar = F)
dim_stations <- ncdim_def("num_stations", "", 1:num_stations, create_dimvar = F)
dim_times <- ncdim_def("num_times", "", 1:num_times, create_dimvar = F)
dim_chars <- ncdim_def('num_chars', "", 1:num_chars, create_dimvar = F)

var_times <- ncvar_def("Times", "second", list(dim_times))
var_station_names <- ncvar_def("StationNames", "char", list(dim_chars, dim_stations), prec = 'char')
var_circulars <- ncvar_def("ParameterCirculars", "char", list(dim_chars, dim_parameters), prec = 'char')
var_parameter_names <- ncvar_def("ParameterNames", "char", list(dim_chars, dim_parameters), prec = 'char')
var_data <- ncvar_def("Data", "", list(dim_parameters, dim_stations, dim_times), missval = NA, prec = 'double')

unlink(file_observations)
ncout <- nc_create(file_observations, list(var_data, var_times, var_station_names, var_parameter_names, var_circulars), force_v4 = T)

ncvar_put(ncout, var_times, value_times)
ncvar_put(ncout, var_station_names, value_station_names)
ncvar_put(ncout, var_parameter_names, value_parameter_names)
ncvar_put(ncout, var_circulars, value_circulars)
ncvar_put(ncout, var_data, obs)
nc_close(ncout)

cat("Observation and foreacst data are generated.\n")
