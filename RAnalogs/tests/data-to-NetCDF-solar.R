# Convert test-Solar data into NetCDF file

library(ncdf4)
library(abind)

rm(list = ls())
load('test-Solar.Rdata')

#########################################################################################
#                                 Convert Forecasts                                     #
#########################################################################################
# Dimensions:
# - num_parameters
# - num_stations
# - num_times
# - num_flts
#
# Variables:
# - Data
# - FLTs
# - Times
# - StationNames
# - ParameterNames
#

file_forecasts <- "solar-forecasts.nc"

num_parameters <- dim(fc)[1]
num_stations <- dim(fc)[2]
num_times <- dim(fc)[3]
num_flts <- dim(fc)[4]
num_chars <- 10

value_xs <- 1:num_stations
value_ys <- 1:num_stations
value_flts <- ((1:num_flts) - 1)
value_times <- ((1:num_times) - 1) * 24 * 60 * 60
value_station_names <- paste("station", 1:num_stations, sep = '')
value_parameter_names <- paste("parameter", 1:num_parameters, sep = '')
value_circulars <- c(value_parameter_names[forecasts.circulars],
                     rep('', num_parameters - length(forecasts.circulars)))

dim_parameters <- ncdim_def("num_parameters", "", 1:num_parameters, create_dimvar = F)
dim_stations <- ncdim_def("num_stations", "", 1:num_stations, create_dimvar = F)
dim_times <- ncdim_def("num_times", "", 1:num_times, create_dimvar = F)
dim_flts <- ncdim_def("num_flts", "", 1:num_flts, create_dimvar = F)
dim_chars <- ncdim_def('num_chars', "", 1:num_chars, create_dimvar = F)

var_xs <- ncvar_def("Xs", "", list(dim_stations))
var_ys <- ncvar_def("Ys", "", list(dim_stations))
var_flts <- ncvar_def("FLTs", "second", list(dim_flts))
var_times <- ncvar_def("Times", "second", list(dim_times))
var_station_names <- ncvar_def("StationNames", "char", list(dim_chars, dim_stations), prec = 'char')
var_circulars <- ncvar_def("ParameterCirculars", "char", list(dim_chars, dim_parameters), prec = 'char')
var_parameter_names <- ncvar_def("ParameterNames", "char", list(dim_chars, dim_parameters), prec = 'char')
var_data <- ncvar_def("Data", "", list(dim_parameters, dim_stations, dim_times, dim_flts), missval = NA, prec = 'double')

unlink(file_forecasts)
ncout <- nc_create(file_forecasts, list(var_flts, var_data, var_times, var_station_names, var_xs, var_ys,
                                        var_parameter_names, var_circulars), force_v4 = T)

ncvar_put(ncout, var_ys, value_ys)
ncvar_put(ncout, var_xs, value_xs)
ncvar_put(ncout, var_flts, value_flts)
ncvar_put(ncout, var_times, value_times)
ncvar_put(ncout, var_station_names, value_station_names)
ncvar_put(ncout, var_parameter_names, value_parameter_names)
ncvar_put(ncout, var_circulars, value_circulars)
ncvar_put(ncout, var_data, fc)
nc_close(ncout)

# ncin <- nc_open(file_forecasts)
# print(ncin)
# nc_close(ncin)


#########################################################################################
#                                 Convert Observations                                  #
#########################################################################################
# Dimensions:
# - num_parameters
# - num_stations
# - num_times
#
# Variables:
# - Data
# - Times
# - StationNames
# - ParameterNames
#

file_observations <- "solar-observations.nc"

num_parameters <- dim(ob)[1]
num_stations <- dim(ob)[2]
num_days <- dim(ob)[3]
num_flts <- dim(ob)[4]
num_times <- num_days * num_flts

value_xs <- 1:num_stations
value_ys <- 1:num_stations
tmp.search.observations <- ob
search.observations <- aperm(tmp.search.observations, c(4, 3, 2, 1))
search.observations <- array(
  search.observations, dim = c(
    dim(tmp.search.observations)[3] * dim(tmp.search.observations)[4], 
    dim(tmp.search.observations)[2],dim(tmp.search.observations)[1]))
value_data <- aperm(search.observations, c(3, 2, 1))

value_station_names <- paste("station", 1:num_stations, sep = '')
value_parameter_names <- paste("parameter", 1:num_parameters, sep = '')
value_times <- rep((1:num_days - 1) * 24 * 60 * 60, each = num_flts) + (1:num_flts - 1)
assertthat::are_equal(length(value_times), num_times)

dim_parameters <- ncdim_def("num_parameters", "", 1:num_parameters, create_dimvar = F)
dim_stations <- ncdim_def("num_stations", "", 1:num_stations, create_dimvar = F)
dim_times <- ncdim_def("num_times", "", 1:num_times, create_dimvar = F)

var_xs <- ncvar_def("Xs", "", list(dim_stations))
var_ys <- ncvar_def("Ys", "", list(dim_stations))
var_times <- ncvar_def("Times", "second", list(dim_times), prec = 'double')
var_station_names <- ncvar_def("StationNames", "char", list(dim_chars, dim_stations), prec = 'char')
var_parameter_names <- ncvar_def("ParameterNames", "char", list(dim_chars, dim_parameters), prec = 'char')
var_data <- ncvar_def("Data", "", list(dim_parameters, dim_stations, dim_times), missval = NA, prec = 'double')

unlink(file_observations)
ncout <- nc_create(file_observations, list(var_data, var_times, var_station_names, var_xs, var_ys, var_parameter_names), force_v4 = T)

ncvar_put(ncout, var_ys, value_ys)
ncvar_put(ncout, var_xs, value_xs)
ncvar_put(ncout, var_times, value_times)
ncvar_put(ncout, var_station_names, value_station_names)
ncvar_put(ncout, var_parameter_names, value_parameter_names)
ncvar_put(ncout, var_data, value_data)
nc_close(ncout)

# ncin <- nc_open(file_observations)
# print(ncin)
# nc_close(ncin)