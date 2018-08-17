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

setwd('~/github/AnalogsEnsemble/RAnalogs/test/')
library(RAnEn)

##################################################################################
#                                 Test 1: Wind                                   #
##################################################################################
rm(list = ls())
load('test-Wind.Rdata')

fc <- fc[, 1, , , drop = F]
ob <- ob[1, , , drop = F]

dim(ob) <- c(1, dim(ob))
test.forecasts <- fc[, , test.start:test.end, , drop = F]
search.forecasts <- fc[, , train.start:train.end, , drop = F]

tmp.search.observations <- ob[, , train.start:train.end, , drop = F]
search.observations <- aperm(tmp.search.observations, c(4, 3, 2, 1))
search.observations <- array(search.observations,
														 dim = c(dim(tmp.search.observations)[3] 
														 				* dim(tmp.search.observations)[4], 
														 				dim(tmp.search.observations)[2],
														 				dim(tmp.search.observations)[1]))
search.observations <- aperm(search.observations, c(3, 2, 1))
rm(tmp.search.observations)

search.times <- (1:dim(search.forecasts)[3]) * 100
search.flts <- 1:dim(search.forecasts)[4]
observation.times <- rep(search.times, each = length(search.flts)) + search.flts

AnEn.cpp <- generateAnalogs(
	test_forecasts = test.forecasts,
	search_forecasts = search.forecasts,
	search_times = search.times,
	search_flts = search.flts,
	search_observations = search.observations,
	observation_times = observation.times,
	num_members = members.size,
	circulars = forecasts.circulars,
	quick = F,
	preserve_similarity = T,
	verbose = 1
)

analogs.cpp <- AnEn.cpp$analogs[,,,,3]

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
                            verbose = 0)

analogs_WU = analogs_c$analogs[1,,,,1]

if (!identical(analogs_WU[1,,],analogs.java[1,,])) {
    stop('Solar test failed!')
}

print("You survived the Wind and the Solar tests!")
