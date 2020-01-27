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

# This script is used for manual test procedure to ensure that the results from
# lastest version of R API development is consistent with the standard results. This
# serves as a way to avoid bugs.
#
# Datasets used in this test include:
# - Wind dataset from Luca research
# - Solar dataset
#
# If the test result is `TRUE` which suggests the results from the development
# version and the standard results are the same, the development version 
# theoretically should be free of bugs.
#

# Set this directory to where the data reside
# setwd('~/github/AnalogsEnsemble/RAnalogs/tests/')

library(RAnEn)

##################################################################################
#                                 Test 1: Wind                                   #
##################################################################################
rm(list = ls())
load('test-Wind.Rdata')

dim(ob) <- c(1, dim(ob))

tmp.search.observations <- ob[, , train.start:train.end, , drop = F]
search.observations <- aperm(tmp.search.observations, c(4, 3, 2, 1))
search.observations <- array(search.observations,
                             dim = c(dim(tmp.search.observations)[3] 
                                     * dim(tmp.search.observations)[4], 
                                     dim(tmp.search.observations)[2],
                                     dim(tmp.search.observations)[1]))
search.observations <- aperm(search.observations, c(3, 2, 1))
rm(tmp.search.observations)

test.times <- (test.start:test.end) * 100
search.times <- (train.start:train.end) * 100
search.flts <- 1:dim(fc)[4]
observation.times <- rep(search.times, each = length(search.flts)) + search.flts

config <- generateConfiguration('independentSearch')
config$forecasts <- fc
config$forecast_times <- c(search.times, test.times)
config$flts <- search.flts
config$observations <- search.observations
config$observation_times <- observation.times
config$num_members <- members.size
config$quick <- F
config$verbose <- 2
config$circulars <- forecasts.circulars
config$search_times <- search.times
config$test_times <- test.times

AnEn.cpp <- generateAnalogs(config)

analogs.cpp <- AnEn.cpp$analogs

if(!identical(analogs.cpp, analogs.java)) {
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

test.times <- (test.start:728) * 1000
search.times <- (train.start:train.end) * 1000
search.flts <- 1:dim(fc)[4]
observation.times <- rep(search.times, each = length(search.flts)) + search.flts

config <- generateConfiguration('independentSearch')
config$forecasts <- fc
config$forecast_times <- c(search.times, (test.start:test.end) * 1000)
config$flts <- search.flts
config$observations <- search.observations
config$observation_times <- observation.times
config$num_members <- members.size
config$quick <- F
config$verbose <- 2
config$circulars <- forecasts.circulars
config$search_times <- search.times
config$test_times <- test.times
config$max_flt_nan <- 100
config$max_par_nan <- 100

AnEn.cpp <- generateAnalogs(config)

analogs_WU <- AnEn.cpp$analogs[1, , , ]

if (!identical(analogs_WU, analogs.java[1:363, , ])) {
  stop('Solar test failed!')
}

cat("You survived the Wind and the Solar tests for R API!\n")