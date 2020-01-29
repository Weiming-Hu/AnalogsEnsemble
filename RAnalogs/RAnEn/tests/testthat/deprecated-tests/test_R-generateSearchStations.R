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

# This script is used to test the search space extension funcionality
rm(list = ls())
library(RAnEn)

load('data-generate-search-stations.Rdata')

test_stations_x <- search_stations_x
test_stations_y <- search_stations_y
stations_tag <- as.numeric(cut(search_stations_x, breaks = num.breaks))
test_stations_classifier <- stations_tag
search_stations_classifier <- stations_tag

num_nearest <- 0
distance <- 100
mat <- generateSearchStations(
  test_stations_x, test_stations_y,
  search_stations_x, search_stations_y,
  100, distance, 
  test_stations_classifier = test_stations_classifier,
  search_stations_classifier = search_stations_classifier)
stopifnot(identical(mat.1, mat))

num_nearest <- 30
distance <- 0
mat <- generateSearchStations(
  test_stations_x, test_stations_y,
  search_stations_x, search_stations_y,
  100, num_nearest = num_nearest, 
  test_stations_classifier = test_stations_classifier,
  search_stations_classifier = search_stations_classifier)
stopifnot(identical(mat.2, mat))

num_nearest <- 10
distance <- 100
mat <- generateSearchStations(
  test_stations_x, test_stations_y,
  search_stations_x, search_stations_y,
  100, distance, num_nearest, 
  stations_tag, stations_tag)
stopifnot(identical(mat.3, mat))

cat("You survived tests for generateSearchStations!\n")

if (F) {
  i.station <- 100
  coords <- generateCircleCoords(test_stations_x[i.station], test_stations_y[i.station], distance)
  
  plot(test_stations_x, test_stations_y, pch = 19, xlab = 'X', ylab = 'Y', asp = 1,
       col = rep(c('blue', 'green'), num.breaks/2)[stations_tag])
  points(test_stations_x[i.station], test_stations_y[i.station], pch = 8, cex = 3)
  lines(coords, lty = 'dotted')
  points(search_stations_x[mat[, i.station]], search_stations_y[mat[, i.station]],
         pch = 1, cex = 2, lty = 'dotted')
}

rm(list = ls())