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

forecasts.time <- 1:num.days * 10
forecasts.flt <- seq(0, by = 3/num.flts, length.out = num.flts) * 10
observations.time <- unique(rep(forecasts.time, each = num.flts) + forecasts.flt)

forecasts <- array(rnorm(num.pars * num.flts * num.days * num.stations, 0, 100),
                   dim = c(num.pars, num.stations, num.days, num.flts))
observations <- array(runif(1 * num.flts * length(observations.time), -10, 10),
                      dim = c(1, num.stations, length(observations.time)))

# Case: operational search + automatic removal
test.start <- 90
test.end <- 94
search.start <- 3
search.end <- 89

config <- generateConfiguration('independentSearch')
config$forecasts <- forecasts
config$forecast_times <- forecasts.time
config$test_times_compare <- forecasts.time[test.start:test.end]
config$search_times_compare <- forecasts.time[search.start:search.end]
config$flts <- forecasts.flt
config$search_observations <- observations
config$observation_times <- observations.time
config$num_members <- num.members
config$operational <- T
config$preserve_similarity <- T
config$max_num_sims <- 100
config$quick <- F
config$verbose <- 0

AnEn.auto <- generateAnalogs(config)

for (station.i in 1:dim(AnEn.auto$similarity_time_index)[1]) {
  for (time.i in 1:dim(AnEn.auto$similarity_time_index)[2]) {
    for (flt.i in 1:dim(AnEn.auto$similarity_time_index)[3]) {
      # Make sure that there are correct numbers of search
      num.valid.similarity <- length(which(!is.na(AnEn.auto$similarity_time_index[station.i, time.i, flt.i, ])))
      num.valid.correct <- ceiling((config$test_times[time.i] - config$flts[flt.i]) / 10) - search.start
      stopifnot(num.valid.similarity == num.valid.correct)
    }
  }
}

cat("You survived the tests for rolling!\n")

