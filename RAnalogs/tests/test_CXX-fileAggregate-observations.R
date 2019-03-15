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

# This script is to test fileAggregate for observations
#

library(abind)
library(ncdf4)

nc <- nc_open('../../tests/Data/observations_part1.nc')
par.names.1.r <- ncvar_get(nc, 'ParameterNames')
xs.1.r <- ncvar_get(nc, 'StationNames')
times.1.r <- ncvar_get(nc, 'Times')
data.1.r <- ncvar_get(nc, 'Data')
nc_close(nc)

nc <- nc_open('../../tests/Data/observations_part2.nc')
par.names.2.r <- ncvar_get(nc, 'ParameterNames')
xs.2.r <- ncvar_get(nc, 'StationNames')
times.2.r <- ncvar_get(nc, 'Times')
data.2.r <- ncvar_get(nc, 'Data')
nc_close(nc)

unlink('obs_c.nc')

# Append along parameters
system('../../output/bin/fileAggregate -t Observations -i ../../tests/Data/observations_part1.nc ../../tests/Data/observations_part2.nc --along 0 -o obs_c.nc')

nc <- nc_open('obs_c.nc')
par.names.c <- ncvar_get(nc, 'ParameterNames')
xs.c <- ncvar_get(nc, 'StationNames')
times.c <- ncvar_get(nc, 'Times')
data.c <- ncvar_get(nc, 'Data')
nc_close(nc)

data.r <- abind(data.1.r, data.2.r, along = 1)
par.names.r <- c(par.names.1.r, par.names.2.r)

ret <- identical(times.c, times.1.r) && identical(xs.c, xs.1.r) &&
  identical(as.vector(par.names.r), as.vector(par.names.c)) &&
  identical(as.vector(data.r), as.vector(data.c))

unlink('obs_c.nc')

if (!ret) {
  stop("File aggregate does failed at processing observations along parameters.")
}

# Append along stations
system('../../output/bin/fileAggregate -t Observations -i ../../tests/Data/observations_part1.nc ../../tests/Data/observations_part2.nc --along 1 -o obs_c.nc')

nc <- nc_open('obs_c.nc')
par.names.c <- ncvar_get(nc, 'ParameterNames')
xs.c <- ncvar_get(nc, 'StationNames')
times.c <- ncvar_get(nc, 'Times')
data.c <- ncvar_get(nc, 'Data')
nc_close(nc)

data.r <- abind(data.1.r, data.2.r, along = 2)
xs.r <- c(xs.1.r, xs.2.r)

ret <- identical(times.c, times.1.r) && identical(par.names.1.r, par.names.c) &&
  identical(as.vector(xs.r), as.vector(xs.c)) &&
  identical(as.vector(data.r), as.vector(data.c))

unlink('obs_c.nc')

if (!ret) {
  stop("File aggregate does failed at processing observations along stations")
}

# Append along times
system('../../output/bin/fileAggregate -t Observations -i ../../tests/Data/observations_part2.nc ../../tests/Data/observations_part1.nc --along 2 -o obs_c.nc')

nc <- nc_open('obs_c.nc')
par.names.c <- ncvar_get(nc, 'ParameterNames')
xs.c <- ncvar_get(nc, 'StationNames')
times.c <- ncvar_get(nc, 'Times')
data.c <- ncvar_get(nc, 'Data')
nc_close(nc)

data.r <- abind(data.1.r, data.2.r, along = 3)
times.r <- c(times.1.r, times.2.r)

ret <- identical(par.names.c, par.names.1.r) && identical(xs.c, xs.1.r) &&
  identical(as.vector(times.r), as.vector(times.c)) &&
  identical(as.vector(data.r), as.vector(data.c))

unlink('obs_c.nc')

if (!ret) {
  stop("File aggregate does failed at processing observations along parameters.")
}

cat("Pass tests of aggregating observations along different dimensions.\n")