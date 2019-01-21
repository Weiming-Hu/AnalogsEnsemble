# Generate forecast part files

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
#
# Variables:
# - Data
# - FLTs
# - Times
# - StationNames
# - ParameterNames
#

file_prefix <- 'forecasts-part-'

num_parameters <- 5
num_stations <- 10
num_times_seq <- 20:30
num_flts <- 2
num_chars <- 10

start_time <- 0
for (i in 1:length(num_times_seq)) {
  
  file_forecasts <- paste(file_prefix, i, '.nc', sep = '')
  num_times <- num_times_seq[i]
  
  fc <- runif(num_parameters * num_stations * num_times * num_flts,
              num_parameters * 10, num_parameters * 100)
  
  value_flts <- ((1:num_flts) - 1) * 3 * 60 * 60
  value_times <- ((1:num_times) - 1 + start_time) * 24 * 60 * 60
  value_station_names <- paste("station", 1:num_stations, sep = '')
  value_xs <- 1:num_stations
  value_ys <- 1:num_stations
  value_parameter_names <- paste("parameter", 1:num_parameters, sep = '')
  value_circulars <- rep('', num_parameters)
  
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
  ncout <- nc_create(file_forecasts, list(var_flts, var_data, var_times, var_station_names, var_xs, var_ys, var_parameter_names, var_circulars), force_v4 = T)
  
  ncvar_put(ncout, var_ys, value_ys)
  ncvar_put(ncout, var_xs, value_xs)
  ncvar_put(ncout, var_flts, value_flts)
  ncvar_put(ncout, var_times, value_times)
  ncvar_put(ncout, var_station_names, value_station_names)
  ncvar_put(ncout, var_parameter_names, value_parameter_names)
  ncvar_put(ncout, var_circulars, value_circulars)
  ncvar_put(ncout, var_data, fc)
  nc_close(ncout)
  
  start_time <- start_time + num_times
}
