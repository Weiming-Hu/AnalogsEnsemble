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
# This script is testing the reading and writing functions in RAnEn

library(RAnEn)

functions.to.test <- c(
  'readForecasts', 'readObservations', 'readConfig',
  'writeNetCDF', 'writeConfig', 'readNc', 'readNc2',
  'readNcAttrs', 'writeNcVar')

# Check whether there are any functions that I have not included
# here in the list. If any, I need to update this test script to
# included the newly added read/write functions.
#
all.funcs <- lsf.str("package:RAnEn")
selected <- grep('^(read|write).*', all.funcs)
if (length(functions.to.test) != length(selected)) {
  stop("Some functions are missing")
}

##########################################
# Test reading and writting observations #
##########################################
#
file.observations <- 'observations.nc'
observations <- list(
  ParameterNames = letters[1:3],
  ParameterCirculars = letters[2],
  Xs = 1:4, Ys = 5:8,
  Times = seq(1, 50, by = 1),
  Data = array(1:600, dim = c(3, 4, 50))
)

observations$Data[sample(120, 100)] <- NA

unlink(file.observations)
writeNetCDF('Observations', observations, file.observations)

observations.read <- readObservations(file.observations)
observations.read$Times <- as.numeric(observations.read$Times)

stopifnot(all.equal(observations.read$Data, observations$Data))
stopifnot(all.equal(observations.read$Times, observations$Times))
stopifnot(all.equal(observations.read$Xs, observations$Xs))
stopifnot(all.equal(observations.read$Ys, observations$Ys))
stopifnot(all.equal(observations.read$ParameterCirculars,
                    observations$ParameterCirculars))
stopifnot(all.equal(observations.read$ParameterNames,
                    observations$ParameterNames))
unlink(file.observations)


#######################################
# Test reading and writting forecasts #
#######################################
#
file.forecasts <- 'forecasts.nc'
forecasts <- list(
  ParameterNames = letters[1:3],
  StationNames = paste0('s', 2:5),
  ParameterCirculars = letters[2],
  Xs = 1:4, Ys = 5:8,
  Times = seq(1, 50, by = 5),
  FLTs = 0:3,
  Data = array(1:480, dim = c(3, 4, 10, 4))
)

forecasts$Data[sample(360, 100)] <- NA

unlink(file.forecasts)
writeNetCDF('Forecasts', forecasts, file.forecasts)

forecasts.read <- readForecasts(file.forecasts)
forecasts.read$Times <- as.numeric(forecasts.read$Times)

stopifnot(all.equal(forecasts.read$Data, forecasts$Data))
stopifnot(all.equal(forecasts.read$Times, forecasts$Times))
stopifnot(all.equal(as.vector(forecasts.read$FLTs), forecasts$FLTs))
stopifnot(all.equal(forecasts.read$Xs, forecasts$Xs))
stopifnot(all.equal(forecasts.read$Ys, forecasts$Ys))
stopifnot(all.equal(forecasts.read$ParameterCirculars,
                    forecasts$ParameterCirculars))
stopifnot(all.equal(forecasts.read$ParameterNames,
                    forecasts$ParameterNames))
stopifnot(all.equal(forecasts.read$StationNames,
                    forecasts$StationNames))
unlink(file.forecasts)

cat("You survived the tests for R file I/O!\n")
