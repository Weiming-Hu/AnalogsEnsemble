#
#  "`-''-/").___..--''"`-._
# (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
# (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#   _ ..`--'_..-_/  /--'_.' ,'
# (il),-''  (li),'  ((!.-'
#
#
# Author: Guido Cervone (cervone@psu.edu)
#         Laura Clemente-Harding (laura@psu.edu)
#         Weiming Hu (weiming@psu.edu)
#
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#
# This script is created for generating AnEn input forecast file

library(ncdf4)

#################################################################
#                        Prepare Data                           #
#################################################################
#
# Notice that variables with NA values will not be written, except
# that some required variables can't be NA.
#
output.name <- 'forecasts.nc'

# Metadata
md.author   <-  "Guido Cervone, Laura Clemente-Harding, and Weiming Hu"
md.contacts <-  "({cervone,laura,weiming}@psu.edu)"
md.unit     <-  "Geoinformatics and Earth Observation Laboratory"
md.web      <-  "http://geolab.psu.edu"
md.inst     <-  "The Pennsylvania State University"

# Dimension length
nchars.max   <- 50
dim.stations <- 20
dim.times    <- 10
dim.pars     <- 5
dim.flts     <- 8

# The origin (starting point) of time
origin <- "1970-01-01"

# Define the grid dimensions in the x and y columns if applicable.  
nx <- NA
ny <- NA

times <- 1:(dim.times*dim.flts)

Xs <- 1:dim.stations
Ys <- dim.stations:1

data <- array(1:(dim.stations*dim.times*dim.flts*dim.pars),
							dim = c(dim.pars, dim.stations, dim.times, dim.flts))

stations.tz   <- paste('timezone_', 1:dim.stations, sep = '')
station.names <- paste('station_', 1:dim.stations, sep = '')

par.names <- paste('par_', 1:dim.pars, sep = '')

units.stations  <- "Number"
units.params    <- "Character"
units.times     <- "Second"
units.lon       <- "Degrees"
units.lat       <- "Degrees"
units.tz        <- "Olson Names"


#################################################################
#                      Write NetCDF file                        #
#################################################################

# Let's write the netcdf for the forecasts 
#
nc.dim.stations  <- ncdim_def(
	"num_stations", "", 1:dim.stations,
	longname="Number of Stations",
	create_dimvar = F)
nc.dim.times     <- ncdim_def(
	"num_times", "",  1:dim.times,
	create_dimvar = F)
nc.dim.params    <- ncdim_def(
	"num_parameters", "", 1:dim.pars,
	create_dimvar = F)
nc.dim.char      <- ncdim_def(
	"num_chars", "", 1:nchars.max, 
	create_dimvar = F)
nc.dim.flts      <- ncdim_def(
	"num_flts", "", 1:dim.flts,
	create_dimvar = F)

# variable definition
nc.var.data      <- ncvar_def("Data", "", list(
	nc.dim.params, nc.dim.stations, nc.dim.times, nc.dim.flts),
	NA, prec="double")
nc.var.times    <- ncvar_def("Times", units.times, list(
	nc.dim.times), NA, longname = paste(
		"Number of seconds since",origin,"00:00:00"), prec = "double")
nc.var.params   <- ncvar_def("ParameterNames", "", list(
	nc.dim.char, nc.dim.params), prec="char")
nc.var.stations <- ncvar_def("StationNames", "", list(
	nc.dim.char, nc.dim.stations), prec="char")
nc.var.tz       <- ncvar_def("TimeZones", units.tz, list(
	nc.dim.char, nc.dim.stations), prec="char")
nc.var.lon      <- ncvar_def("Xs", units.lon, nc.dim.stations)
nc.var.lat      <- ncvar_def("Ys", units.lat, nc.dim.stations)

# Removing the file if it already exists
unlink(output.name)

nc.new <- nc_create(output.name, list(
	nc.var.data, nc.var.params, nc.var.tz, nc.var.lon,
	nc.var.lat, nc.var.times, nc.var.stations))

ncvar_put(nc.new, nc.var.data, data, start=c(1,1,1,1))
ncvar_put(nc.new, nc.var.params, par.names)
ncvar_put(nc.new, nc.var.tz, stations.tz)
ncvar_put(nc.new, nc.var.lon, Xs)
ncvar_put(nc.new, nc.var.lat, Ys)
ncvar_put(nc.new, nc.var.times, times)
ncvar_put(nc.new, nc.var.stations, station.names)

if ( !is.na(nx) && !is.na(ny) ) {
	ncatt_put( nc.new, "Data", "nx", nx, "short")
	ncatt_put( nc.new, "Data", "ny", ny, "short")
}

ncatt_put(nc.new, 0, 'Authors', md.author)
ncatt_put(nc.new, 0, 'Contacts', md.contacts)
ncatt_put(nc.new, 0, 'Unit', md.unit)
ncatt_put(nc.new, 0, 'Web', md.web)
ncatt_put(nc.new, 0, 'Institute', md.inst)
ncatt_put(nc.new, 0, 'Created',
					format(Sys.time(), format = '%Y-%m-%d_%H:%M:%S_%Z'))

nc_close( nc.new ) 
