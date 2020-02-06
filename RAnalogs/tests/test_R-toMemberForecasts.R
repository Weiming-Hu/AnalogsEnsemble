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

# This script tests RAnEn::toMemberForecasts

library(RAnEn)

# Set up analog generation with IS
config <- new(Config)
config$save_analogs <- F
config$num_similarity <- 20
config$save_similarity_time_index <- T

forecasts <- generateForecastsTemplate()
forecasts$Data <- array(runif(21000), dim = c(3, 20, 50, 7))
forecasts$Times <- as.POSIXct((1:50) * 10, origin = '1970-01-01')
forecasts$FLTs <- array(0:6, dim = 7)

observations <- generateObservationsTemplate()
observations$Data <- array(runif(12000), dim = c(1, 20, 600))
observations$Times <- 1:600

AnEn <- generateAnalogs(forecasts, observations, (41:50) * 10, (1:40) * 10, sx_config = config)

sims.time.index <- AnEn$similarity_time_index
sims.stations.index <- AnEn$sims.stations.index
forecasts <- forecasts$Data

# Get the historical foreacsts associated with particular analogs
stations.index <- 2:15
test.times.index <- 2:4
flts.index <- 3:5
parameters.index <- 2:3
members.index <- 2:5
verbose <- F

historicals <- toMemberForecasts(
  forecasts = forecasts,
  sims.time.index = sims.time.index,
  stations.index = stations.index,
  test.times.index = test.times.index,
  flts.index = flts.index,
  parameters.index = parameters.index,
  members.index = members.index,
  verbose = verbose)

for (station.i in 1:length(stations.index)) {
  station.index <- stations.index[station.i]
  
  for (time.i in 1:length(test.times.index)) {
    test.time.index <- test.times.index[time.i]
    
    for (flt.i in 1:length(flts.index)) {
      flt.index <- flts.index[flt.i]
      
      for (member.i in 1:length(members.index)) {
        member.index <- members.index[member.i]
        
        for (parameter.i in 1:length(parameters.index)) {
          parameter.index <- parameters.index[parameter.i]
          
          index <- AnEn$similarity_time_index[station.index, test.time.index, flt.index, member.index]
          manual.fcst.value <- forecasts[parameter.index, station.index, index, flt.index]
          auto.fcst.value <- historicals[parameter.i, station.i, time.i, flt.i, member.i]
          
          stopifnot(auto.fcst.value == manual.fcst.value)
        }
      }
    }
  }
}

cat("You survived the tests for RAnEn::toMemberForecasts!\n")
