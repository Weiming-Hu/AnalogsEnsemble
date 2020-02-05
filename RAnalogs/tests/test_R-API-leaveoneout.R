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

# This script tests leave one out.

# This scripts also tests the deprecated versions.

library(RAnEn)

# Prepare test data
num.pars <- 3
num.flts <- 6
num.days <- 100
num.members <- 5
num.stations <- 9

forecasts.time <- 1:num.days * 10
forecasts.flt <- seq(0, by = 3/num.flts, length.out = num.flts) * 10
observations.time <- unique(rep(forecasts.time, each = num.flts) + forecasts.flt)

forecasts <- array(rnorm(num.pars * num.flts * num.days * num.stations, 0, 100),
                   dim = c(num.pars, num.stations, num.days, num.flts))
observations <- array(runif(1 * num.flts * length(observations.time), -10, 10),
                      dim = c(1, num.stations, length(observations.time)))

# Case: leave one out with the latest version
test.start <- 90
test.end <- 92
search.start <- 1
search.end <- 100

config <- generateConfiguration('independentSearch')
config$forecasts <- forecasts
config$forecast_times <- forecasts.time
config$test_times_compare <- forecasts.time[test.start:test.end]
config$search_times_compare <- forecasts.time[search.start:search.end]
config$flts <- forecasts.flt
config$search_observations <- observations
config$observation_times <- observations.time
config$num_members <- num.members
config$prevent_search_future <- F
config$preserve_similarity <- T
config$preserve_similarity_index <- T
config$max_num_sims <- 100
config$quick <- F
config$verbose <- 0

AnEn.auto <- generateAnalogs(config)

for (station.i in 1:dim(AnEn.auto$similarity)[1]) {
  for (time.i in 1:dim(AnEn.auto$similarity)[2]) {
    for (flt.i in 1:dim(AnEn.auto$similarity)[3]) {
      # Make sure that the correct day is removed
      sorted.search.days <- sort(AnEn.auto$similarity[station.i, time.i, flt.i, ,3])
      appended.search.days <- sort(c(sorted.search.days, config$test_times[time.i]/10))
      stopifnot(all.equal(appended.search.days, 1:100))
    }
  }
}

cat("You survived the tests for leave-one-out search!\n")
