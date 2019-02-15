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

# This script tests rolling.

# This scripts also tests the deprecated versions.

library(RAnEn)

# Prepare test data
num.pars <- 3
num.flts <- 6
num.days <- 100
num.members <- 5
num.stations <- 9

forecasts.time <- 1:num.days
forecasts.flt <- seq(0, by = 3/num.flts, length.out = num.flts)
observations.time <- unique(rep(forecasts.time, each = num.flts) + forecasts.flt)

forecasts <- array(rnorm(num.pars * num.flts * num.days * num.stations, 0, 100),
                   dim = c(num.pars, num.stations, num.days, num.flts))
observations <- array(runif(1 * num.flts * length(observations.time), -10, 10),
                      dim = c(1, num.stations, length(observations.time)))

# Case: operational search + automatic removal
test.start <- 90
test.end <- 94
search.start <- 1
search.end <- 94

config <- generateConfiguration('independentSearch', TRUE)
config$test_forecasts <- forecasts
config$test_times <- forecasts.time
config$test_times_compare <- forecasts.time[test.start:test.end]
config$search_forecasts <- forecasts
config$search_times <- forecasts.time
config$search_times_compare <- forecasts.time[search.start:search.end]
config$flts <- forecasts.flt
config$search_observations <- observations
config$observation_times <- observations.time
config$num_members <- num.members
config$operational <- T
config$preserve_similarity <- T
config$quick <- F

AnEn.auto <- generateAnalogs(config)

# Case: Deprecated functions
observations4D <- array(dim = c(1, dim(forecasts)[2:4]))
for (i.time in 1:dim(observations4D)[3]) {
  for (i.flt in 1:dim(observations4D)[4]) {
    i.obs <- AnEn.auto$mapping[i.flt, i.time]
    
    for (i.par in 1:dim(observations4D)[1]) {
      for (i.station in 1:dim(observations4D)[2]) {
        observations4D[i.par, i.station, i.time, i.flt] <-
          observations[i.par, i.station, i.obs]
      }
    }
    
  }
}

AnEn.dep <- compute_analogs(
  forecasts, observations4D,
  test_ID_start = test.start,
  test_ID_end = test.end,
  train_ID_start = search.start,
  train_ID_end = search.end,
  members_size = num.members,
  rolling = -3,
  quick = F)

for (i.flt in 1:dim(AnEn.dep$analogs)[3]) {
  AnEn.dep$analogs[, , i.flt, , 3] <- AnEn.auto$mapping[i.flt, AnEn.dep$analogs[, , i.flt, , 3]]
}

stopifnot(identical(
  as.vector(AnEn.dep$analogs), 
  as.vector(AnEn.auto$analogs)))

print("Rolling results are the same between latest and deprecated version.")

# Case: Manual removal of overlapping forecasts
test.start <- 94
test.end <- 94
search.start <- 1
search.end <- 91

config$test_times_compare <- forecasts.time[test.start:test.end]
config$search_times_compare <- forecasts.time[search.start:search.end]
config$operational <- F

AnEn.man <- generateAnalogs(config)

# Case: Deprecated functions
AnEn.dep <- compute_analogs(
  forecasts, observations4D,
  test_ID_start = test.start,
  test_ID_end = test.end,
  train_ID_start = search.start,
  train_ID_end = search.end,
  members_size = num.members,
  quick = F)

for (i.flt in 1:dim(AnEn.dep$analogs)[3]) {
  AnEn.dep$analogs[, , i.flt, , 3] <- AnEn.man$mapping[i.flt, AnEn.dep$analogs[, , i.flt, , 3]]
}

# Compare
stopifnot(identical(
  as.vector(AnEn.auto$analogs[, 5, , , ]), 
  as.vector(AnEn.man$analogs)))

print("Rolling results are the same between automatic removal and manual removal.")

stopifnot(identical(
  as.vector(AnEn.dep$analogs[, , , , 1]), 
  as.vector(AnEn.man$analogs[, , , , 1])))

print("Results are the same without rolling between the latest and the deprecated verions.")

cat("You survived the tests for rolling!\n")
