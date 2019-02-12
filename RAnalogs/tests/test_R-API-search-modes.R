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

# This script tests the different search modes of AnEn, including
# - Leave One Out
# - Operational Search
# - Separate Test and Search
#
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

# Case: Automatic removal of overlapping forecasts
test.start <- 90
test.end <- 90
search.start <- 1
search.end <- 89

config <- generateConfiguration('independentSearch')
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
config$preserve_similarity <- T
config$quick <- F

AnEn.auto <- generateAnalogs(config)

# Case: Manual removal of overlapping forecasts
test.start <- 90
test.end <- 90
search.start <- 1
search.end <- 87

config <- generateConfiguration('independentSearch')
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
config$preserve_similarity <- T
config$quick <- F

AnEn.man <- generateAnalogs(config)

analogs <- AnEn.auto$analogs
sims <- AnEn.auto$similarity

sims <- aperm(sims, 5:1)

analogs[4, 1, 3, ,]
sims[4, 1, 3, 1:10,]

forecasts.time[40]
observations.time[81]

# Compare
stopifnot(identical(AnEn.auto$analogs, AnEn.man$analogs))



cat("You survived the tests for search modes!\n")
