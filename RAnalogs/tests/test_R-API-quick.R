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

# This script tests whether the quick sort algorithm is working.
# 
# Basically, analog similairty (which corresponds to the range
# 1:num_analogs) should always remain the lowest.
# 
# If quick sort is not used, the similarity will be sorted within
# the range of 1:num_analogs; if quick sort is used, the similarity 
# for the range is not internally sorted.
# 
library(RAnEn)

config <- new(Config)
config$save_similarity <- T
config$save_analogs <- F

forecasts <- generateForecastsTemplate()
forecasts$Data <- array(runif(2500), dim = c(2, 5, 50, 5))
forecasts$Times <- as.POSIXct(1:50, origin = '1970-01-01')
forecasts$FLTs <- array(0:4, dim = 5)

observations <- generateObservationsTemplate()
observations$Data <- array(runif(1500), dim = c(1, 5, 300))
observations$Times <- 1:300

# Save more entries in analogs and similarity. Quick sort
# is not used so that 1:num_analogs should be sorted. But
# num_analogs:num_similarity is not necessarily sorted.
# 
num_analogs_1 <- 35
num_analogs_2 <- 10

config$num_analogs <- num_analogs_1
config$num_similarity <- 40
config$quick <- F

# AnEn with no circulars
AnEn <- generateAnalogs(forecasts, observations, 41:50, 1:40, sx_config = config)

if (is.unsorted(AnEn$similarity[1, 1, 1, 1:config$num_analogs])) {
  stop('Similarity for analogs are not sorted. Check your algorithm for quick sort.')
}

# Run the algorithm with the same configuration except for quick sort
# and the number of analogs
# 
config$quick <- T
config$num_analogs <- num_analogs_2

# AnEn with no circulars
AnEn.quick <- generateAnalogs(forecasts, observations, 41:50, 1:40, sx_config = config)

if (!is.unsorted(AnEn.quick$similarity[1, 1, 1, 1:config$num_analogs])) {
  stop("Wierd. I don't want analog similarity to be sorted but they appear to be so somehow.")
}

if (!identical(sort(AnEn.quick$similarity[1, 1, 1, 1:config$num_analogs]),
               AnEn$similarity[1, 1, 1, 1:config$num_analogs])) {
  stop("Wierd. Looks like the analog similarity is not the global minimal.")
}

if (interactive()) {
  plot(AnEn.quick$similarity[1, 1, 1, ], type = 'b',
       main = 'nth_element (Points to the left of the red line should be the lowest unsorted)')
  abline(v = num_analogs_2, col = 'red')
  plot(AnEn$similarity[1, 1, 1, ], type = 'b',
       main = 'partial_sort (Points to the left of the red line should be the lowest sorted)')
  abline(v = num_analogs_1, col = 'red')
}


cat("You survived the tests for quick sort!\n")