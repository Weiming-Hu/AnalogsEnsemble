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
# lastest version of development is consistent with the standard results. This
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
setwd('~/github/AnalogsEnsemble/RAnalogs/tests/')

library(RAnEn)

##################################################################################
#                                 Test 1: Wind                                   #
##################################################################################
rm(list = ls())
load('test-Wind.Rdata')

dim(ob) <- c(1, dim(ob))

AnEn.cpp <- compute_analogs(
  forecasts = fc, observations = ob,
  circulars = forecasts.circulars,
  members_size = members.size,
  rolling = rolling,
  quick = as.logical(sort.type),
  test_ID_start = test.start,
  test_ID_end = test.end,
  train_ID_start = train.start,
  train_ID_end = train.end,
  weights = weights,
  verbose = 2
)

analogs.cpp <- AnEn.cpp$analogs[,,,,1]

if(!identical(analogs.cpp, analogs.java)) {
    stop('Wind test failed!')
}


##################################################################################
#                                 Test 2: Solar                                  #
##################################################################################
rm(list = ls())
load('test-Solar.Rdata')

analogs_c = compute_analogs(forecasts = fc,
                            observations = ob,
                            circulars = forecasts.circulars,
                            weights = weights,
                            stations_ID = 1,
                            test_ID_start = test.start,
                            test_ID_end = test.end,
                            train_ID_start = train.start,
                            train_ID_end = train.end,
                            observation_ID = 1,
                            members_size = members.size,
                            rolling = rolling,
                            quick = F,
                            cores = 6,
                            output_search_stations = F,
                            output_metric = F,
                            verbose = 2)

analogs_WU = analogs_c$analogs[1,,,,1]

if (!identical(analogs_WU[1,,],analogs.java[1,,])) {
    stop('Solar test failed!')
}

cat("You survived the Wind and the Solar tests!\n")
