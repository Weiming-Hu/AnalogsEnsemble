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

library(rgdal)
library(tools)
library(ncdf4)

if (Sys.getenv('USER') == 'wuh20') {
  tz.world.fname  <-  "~/Nextcloud/data/tz_world/tz_world.shp"
  wind.fname      <-  "~/Nextcloud/data/data_windLuca/WindDataForAnEn.Rdata"  
} else {
  tz.world.fname  <-  "~/geolab_storage_V3/data/Shapefiles//tz_world/tz_world.shp"
  wind.fname      <-  "~/geolab_storage_V3/data/Analogs/WindDataForAnEn.Rdata"  
}

# For forecasts, wind.fname is the forecasts for all variables 
obs.fname       <-  "ObservationsWind.nc"

md.author       <-  "Guido Cervone, Laura Clemente-Harding, and Weiming Hu"
md.contacts     <-  "({cervone,laura,weiming}@psu.edu)"
md.unit         <-  "Geoinformatics and Earth Observation Laboratory"
md.web          <-  "http://geolab.psu.edu"
md.inst         <-  "The Pennsylvania State University"

basename_sans_ext <- function( x ) { return( basename(file_path_sans_ext(x)) ) }


# Load the original wind data
load(wind.fname)

# The maximum string size for the data you can have. For example, this is the maximum string size
#  of space that a value can take up - whether character, parameter value, etc.
#
nchars.max    <- 50   

# Let's define the times for when these data have been collected
# epoch time (also called Unix time or POSIX time or Unix timestamp is the seconds that have 
# elapsed since 1970 - although the origin time could be changed, 1970 is the standard)
#
origin       <- "1970-01-01"
secs.interval <- 3 * 60 * 60

date.start   <- as.POSIXct("2010-05-01", origin=origin, tz="UTC")

# There are 17 flts in observations 4D, which is 8 + 8 + 1 intervals.
# To -1 makes the range not inclusive
#
date.end     <- date.start + ((dim(observations)[3] + 1) * 8 + 1) * secs.interval - 1


# Define the grid dimensions in the x and y columns if applicable.  
# If set to NA it will not write them
#
nx = NA
ny = NA

# times are every 3 hours
times        <- seq( as.numeric(date.start), as.numeric(date.end), secs.interval)

# convert 4D observations to 3D
observations.3D <- array(NA, dim = c(dim(observations)[1:2], length(times)))
for (i in 1:dim(observations.3D)[1]) {
  for (j in 1:dim(observations.3D)[2]) {
    
    # get rid of duplication
    tmp <- observations[i, j, ,1:8]
    
    # get the rest of the times from the last row
    tmp <- rbind(tmp, observations[i, j, dim(observations)[3], 9:16])
    
    # add the last time of the array
    tmp <- c(as.vector(t(tmp)), observations[i, j, dim(observations)[3], 17])
    
    observations.3D[i, j, ] <- tmp
  }
}

observations <- observations.3D
rm(observations.3D)

# Now we need to compute the time zone for each of the stations.
#
world.tz.spdf <- readOGR(dirname(tz.world.fname), basename_sans_ext(tz.world.fname))

CRS.longlat  <-  CRS( proj4string( world.tz.spdf) )
stations.sp  <-  SpatialPoints( cbind(longs,lats), proj4string = CRS.longlat )

stations.tz  <- over( stations.sp, world.tz.spdf ) # Extract the tz for each lat/long
stations.tz  <- as.character( stations.tz[,1] )   # convert from factor to string

station.names <- paste('Station_', 1:dim.stations, sep = '')


dim.times       <-  length(times)
dim.stations    <-  dim(observations)[2]  # Really, the number of stations is the same so we could leave this with observations but best to be cleaner 
dim.parameters  <-  dim(observations)[1]  # Definitely different for forecasts 

units.stations  <- "Number"
units.params    <- "Character"
units.times     <- "Second"
units.lon       <- "Degrees"
units.lat       <- "Degrees"
units.tz        <- "Olson Names" # When we read things back in, we can actually just pass the Olson names to R and it can read it 
units.grid      <- "Number"

# Let's write the netcdf for the forecasts 
#
nc.dim.stations  <- ncdim_def( "num_stations", "", 1:dim.stations, longname="Number of Stations", create_dimvar=FALSE)
nc.dim.times     <- ncdim_def( "num_times", "", 1:dim.times, create_dimvar = FALSE)
nc.dim.params    <- ncdim_def( "num_parameters", "", 1:dim.parameters, create_dimvar=FALSE)
nc.dim.char      <- ncdim_def( "num_chars", "", 1:nchars.max, create_dimvar=FALSE )

# variable definition
nc.var.obs      <- ncvar_def( "Data", "", list(nc.dim.params, nc.dim.stations, nc.dim.times),NA,prec="double")
nc.var.times    <- ncvar_def( "Times", units.times, list(nc.dim.times), NA, 
                              longname=paste("Number of seconds since",origin,"00:00:00"), prec = "double")
nc.var.params   <- ncvar_def( "ParameterNames", "", list(nc.dim.char, nc.dim.params), prec="char")
nc.var.stations <- ncvar_def( 'StationNames', "", list(nc.dim.char, nc.dim.stations), prec="char")
nc.var.tz       <- ncvar_def( "TimeZones", units.tz, list(nc.dim.char, nc.dim.stations), prec="char")
nc.var.lon      <- ncvar_def( "Xs", units.lon, nc.dim.stations)
nc.var.lat      <- ncvar_def( "Ys", units.lat, nc.dim.stations)

# Removing the file
unlink(obs.fname)

nc.data.obs    <- nc_create( obs.fname, list(nc.var.obs, nc.var.params, nc.var.tz,
                                            nc.var.lon, nc.var.lat, nc.var.times,
                                            nc.var.stations))
 
ncvar_put( nc.data.obs, nc.var.obs,  observations, start=c(1,1,1))
ncvar_put( nc.data.obs, nc.var.params, observations.names)
ncvar_put( nc.data.obs, nc.var.tz, stations.tz)
ncvar_put( nc.data.obs, nc.var.lon, longs)
ncvar_put( nc.data.obs, nc.var.lat, lats)
ncvar_put( nc.data.obs, nc.var.times, times)
ncvar_put( nc.data.obs, nc.var.stations, station.names)

if ( !is.na(nx) && !is.na(ny) ) {
  ncatt_put( nc.data.obs, "Data", "nx", nx, "short")
  ncatt_put( nc.data.obs, "Data", "ny", ny, "short")
}

ncatt_put(nc.data.obs, 0, 'Authors', md.author)
ncatt_put(nc.data.obs, 0, 'Contacts', md.contacts)
ncatt_put(nc.data.obs, 0, 'Unit', md.unit)
ncatt_put(nc.data.obs, 0, 'Web', md.web)
ncatt_put(nc.data.obs, 0, 'Institute', md.inst)
ncatt_put(nc.data.obs, 0, 'Created',
          format(Sys.time(), format = '%Y-%m-%d_%H:%M:%S_%Z'))

nc_close( nc.data.obs ) 
