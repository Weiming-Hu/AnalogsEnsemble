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

# This script is the same with the test_R-API-old.R except for it uses the v4 APIs.
#

library(RAnEn)

##################################################################################
#                                 Test 1: Wind                                   #
##################################################################################
rm(list = ls())
load('test-Wind.Rdata')

dim(ob) <- c(1, dim(ob))

# Data preprocessing
tmp.search.observations <- ob[, , train.start:train.end, , drop = F]
search.observations <- aperm(tmp.search.observations, c(4, 3, 2, 1))
search.observations <- array(search.observations,
														 dim = c(dim(tmp.search.observations)[3] 
														 				* dim(tmp.search.observations)[4], 
														 				dim(tmp.search.observations)[2],
														 				dim(tmp.search.observations)[1]))
search.observations <- aperm(search.observations, c(3, 2, 1))
rm(tmp.search.observations)

# Test and search times
search.times <- (train.start:train.end) * 100
test.times <- (test.start:test.end) * 100

# Generate forecasts
forecasts <- generateForecastsTemplate()
forecasts$Data <- fc
forecasts$ParameterCirculars <- forecasts.circulars
forecasts$Times <- (train.start:test.end) * 100
forecasts$FLTs <- 1:dim(fc)[4]
forecasts$ParameterCirculars <- forecasts.circulars

# Generate observations
observations <- generateObservationsTemplate()
observations$Data <- search.observations
observations$Times <- rep(search.times, each = length(forecasts$FLTs)) + forecasts$FLTs

# Configuration
config <- new(Config)
config$num_analogs <- members.size
config$quick <- F

AnEn.cpp <- generateAnalogs(forecasts, observations, test.times, search.times, config)

if(!identical(AnEn.cpp$analogs, analogs.java)) {
  stop('Wind test failed!')
}


##################################################################################
#                                 Test 2: Solar                                  #
##################################################################################
rm(list = ls())
load('test-Solar.Rdata')

tmp.search.observations <- ob[, , train.start:train.end, , drop = F]
search.observations <- aperm(tmp.search.observations, c(4, 3, 2, 1))
search.observations <- array(search.observations,
                             dim = c(dim(tmp.search.observations)[3] 
                                     * dim(tmp.search.observations)[4], 
                                     dim(tmp.search.observations)[2],
                                     dim(tmp.search.observations)[1]))
search.observations <- aperm(search.observations, c(3, 2, 1))
rm(tmp.search.observations)

# Test and search times
test.times <- (test.start:728) * 1000
search.times <- (train.start:train.end) * 1000

# Forecasts
search.flts <- 1:dim(fc)[4]
forecasts <- generateForecastsTemplate()
forecasts$Data <- fc
forecasts$Times <- c(search.times, (test.start:test.end) * 1000)
forecasts$FLTs <- search.flts
forecasts$ParameterCirculars <- forecasts.circulars

# Observations
observations <- generateObservationsTemplate()
observations$Times <- rep(search.times, each = length(search.flts)) + search.flts
observations$Data <- search.observations

# Config
config <- new(Config)
config$num_analogs <- members.size
config$quick <- F
config$max_flt_nan <- 100
config$max_par_nan <- 100

AnEn.cpp <- generateAnalogs(forecasts, observations, test.times, search.times, config)

analogs_WU <- AnEn.cpp$analogs[1, , , ]

if (!identical(analogs_WU, analogs.java[1:363, , ])) {
  stop('Solar test failed!')
}

cat("You survived the Wind and the Solar tests for R API v4!\n")