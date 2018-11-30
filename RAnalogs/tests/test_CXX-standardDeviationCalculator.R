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
#
# This script is testing the C++ application standardDeviationCalculator

library(ncdf4)
library(abind)

rm(list = ls())
exe <- '../../output/bin/standardDeviationCalculator'
file_in_1 <- '../../tests/Data/forecasts_part1.nc'
file_in_2 <- '../../tests/Data/forecasts_part2.nc'
file_out <- 'sds.nc'
verbose <- 2

unlink(file_out)
command <- paste(exe, '-i', file_in_1, file_in_2,
                 '-o', file_out,
                 '-v', verbose)

unlink(file_out)
system(command)

nc <- nc_open(file_out)
sds.c <- ncvar_get(nc, "StandardDeviation");
nc_close(nc)

nc <- nc_open(file_in_1)
fcsts.1 <- ncvar_get(nc, 'Data')
nc_close(nc)

nc <- nc_open(file_in_2)
fcsts.2 <- ncvar_get(nc, 'Data')
nc_close(nc)

fcsts <- abind(fcsts.1, fcsts.2, along = 3)
sds.r <- apply(fcsts, c(1, 2, 4), sd, na.rm = T)

dif <- abs(sds.r - sds.c)
dif <- dif[dif < 1e+10]
if (sum(dif, na.rm = T) < 1e-8) {
  print("You passed the test!")
} else {
  stop("Something is wrong.")
}
