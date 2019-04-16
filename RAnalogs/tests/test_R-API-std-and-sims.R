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

# This script test the standard deviation and similarity calculation of the core C++
# library. It does so by comparing the results from C++ and from the calculation here
# manually. These tests are not comprehensive in a sense that it does not test the
# full functionality of C++ functions, for example, search space extension and the 
# operational search technique.
#

rm(list = ls())

library(RAnEn)

num.pars <- 3
num.stations <- 2
num.days <- 50
num.flts <- 5
test.start <- 46
test.end <- 50
search.start <- 1
search.end <- 43

forecasts <- list()
forecasts$Data <- array(data = runif(num.pars*num.stations*num.days*num.flts),
                        dim = c(num.pars, num.stations, num.days, num.flts))
forecasts$Times <- 1:num.days * 100
forecasts$FLTs <- 1:num.flts

observations <- list()
observations$Data <- array(data = runif(num.pars*num.stations*num.days*num.flts),
                           dim = c(num.pars, num.stations, num.days*num.flts))
observations$Times <- rep(forecasts$Times, each = num.flts) + forecasts$FLTs

# A portion of the values will be NA
NA.portion <- 0.1
index <- sample(length(forecasts$Data), size = length(forecasts$Data) * NA.portion)
forecasts$Data[index] <- NA
index <- sample(length(observations$Data), size = length(observations$Data) * NA.portion)
observations$Data[index] <- NA

#########################################################################################
#                               Test 1: Allow any number of NAs                         #
#########################################################################################
# Generate AnEn
config <- generateConfiguration('independentSearch')

config$forecasts <- forecasts$Data
config$forecast_times <- forecasts$Times
config$flts <- forecasts$FLTs
config$search_observations <- observations$Data
config$observation_times <- observations$Times
config$observation_id <- 2
config$num_members <- 20
config$verbose <- 2

config$max_par_nan <- -1
config$max_flt_nan <- -1

config$quick <- F
config$preserve_similarity <- T
config$preserve_std <- T

config$test_times_compare <- forecasts$Times[test.start:test.end]
config$search_times_compare <- forecasts$Times[search.start:search.end]

AnEn <- generateAnalogs(config)

# Check std calculation
search.forecasts <- config$forecasts[, , search.start:search.end, ]
std.R <- apply(search.forecasts, c(1, 2, 4), sd, na.rm = T)

cat("Stadard deviation difference for each parameter:\n")
for (i.par in 1:num.pars) {
  print(range(as.vector(std.R[i.par, , ] - AnEn$std[i.par, , ]), na.rm = T))
  stopifnot(range(as.vector(std.R[i.par, , ] - AnEn$std[i.par, , ])[2]) < 1e-10)
}

# Check similarity calculation
cat("Similarity (allowing any number of NAs) differences:\n")
for (i.test.day in 1:(test.end-test.start+1)) {
  
  for (i.station in 1:num.stations) {
    
    for (i.flt in 2:(num.flts-1)) {
      
      # AnEn$similarity[i.station, i.test.day, i.flt, 1:10, ]
      for (i.sim in 1:config$num_members) {
        
        sim.value <- AnEn$similarity[i.station, i.test.day, i.flt, i.sim, 1]
        
        if (is.na(sim.value)) {
          next
        }
        
        sim.index <- AnEn$similarity[i.station, i.test.day, i.flt, i.sim, 3]
        
        current.fcsts <- config$forecasts[, i.station, (test.start+i.test.day-1), ((i.flt-1):(i.flt+1))]
        search.fcsts <- config$forecasts[, i.station, sim.index, ((i.flt-1):(i.flt+1))]
        
        sim.R <- 0
        for (i.par in 1:num.pars) {
          current.fcsts.par <- current.fcsts[i.par, ]
          search.fcsts.par <- search.fcsts[i.par, ]
          
          diff.par <- mean((search.fcsts.par - current.fcsts.par)^2, na.rm = T)
          
          if (AnEn$std[i.par, i.station, i.flt] != 0 && !is.na(diff.par)) {
            diff.par <- sqrt(diff.par)/AnEn$std[i.par, i.station, i.flt]
            # cat("diff par:", diff.par, '\n')
            sim.R <- sim.R + diff.par
          }
        }
        
        cat(sim.R - sim.value, ',')
        stopifnot(sim.R - sim.value < 1e-10)
      }
    }
  }
}

#########################################################################################
#                               Test 2: Don't allow any number of NAs                   #
#########################################################################################
# Generate AnEn
config$max_par_nan <- 0
config$max_flt_nan <- 0
AnEn <- generateAnalogs(config)

# Check std calculation
search.forecasts <- config$forecasts[, , search.start:search.end, ]
std.R <- apply(search.forecasts, c(1, 2, 4), sd, na.rm = T)

cat("Stadard deviation difference for each parameter:\n")
for (i.par in 1:num.pars) {
  print(range(as.vector(std.R[i.par, , ] - AnEn$std[i.par, , ]), na.rm = T))
  stopifnot(range(as.vector(std.R[i.par, , ] - AnEn$std[i.par, , ])[2]) < 1e-10)
}

# Check similarity calculation
cat("Similarity (allowing no NAs) differences:\n")
for (i.test.day in 1:(test.end-test.start+1)) {
  
  for (i.station in 1:num.stations) {
    
    for (i.flt in 2:(num.flts-1)) {
      
      # AnEn$similarity[i.station, i.test.day, i.flt, 1:10, ]
      for (i.sim in 1:config$num_members) {
        
        sim.value <- AnEn$similarity[i.station, i.test.day, i.flt, i.sim, 1]
        
        if (is.na(sim.value)) {
          next
        }
        
        sim.index <- AnEn$similarity[i.station, i.test.day, i.flt, i.sim, 3]
        
        current.fcsts <- config$forecasts[, i.station, (test.start+i.test.day-1), ((i.flt-1):(i.flt+1))]
        search.fcsts <- config$forecasts[, i.station, sim.index, ((i.flt-1):(i.flt+1))]
        
        sim.R <- 0
        for (i.par in 1:num.pars) {
          current.fcsts.par <- current.fcsts[i.par, ]
          search.fcsts.par <- search.fcsts[i.par, ]
          
          diff.par <- mean((search.fcsts.par - current.fcsts.par)^2)
          
          if (AnEn$std[i.par, i.station, i.flt] != 0) {
            diff.par <- sqrt(diff.par)/AnEn$std[i.par, i.station, i.flt]
            # cat("diff par:", diff.par, '\n')
            sim.R <- sim.R + diff.par
          }
        }
        
        cat(sim.R - sim.value, ',')
        stopifnot(sim.R - sim.value < 1e-10)
      }
    }
  }
}

cat("\nYou passed the tests for standard deviation calculation and the similarity calculation!\n") 