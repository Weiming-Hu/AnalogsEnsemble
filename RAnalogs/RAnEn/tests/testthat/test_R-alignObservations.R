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

# This script tests RAnEn::alignObservations

library(RAnEn)

rm(list = ls())

num.pars <- 1
num.stations <- 1
num.days <- 10
num.flts <- 9

forecast.times <- 1:num.days * 100
flts <- seq(from = 0, length.out = num.flts, by = 20)

observation.times <- unique(rep(forecast.times, each = num.flts) + flts)

obs <- array(observation.times, dim = c(num.pars, num.stations, length(observation.times)))
aligned.obs <- alignObservations(obs, observation.times, forecast.times, flts, show.progress = F)

fcsts <- array(NA, dim = c(num.pars, num.stations, num.days, num.flts))
for (i.days in 1:num.days) {
  fcsts[1, 1, i.days, ] <- observation.times[((i.days-1)*5+1):((i.days-1)*5+num.flts)]
}

stopifnot(identical(fcsts, aligned.obs))

cat("You survived the tests for RAnEn::alignObservations!\n")
