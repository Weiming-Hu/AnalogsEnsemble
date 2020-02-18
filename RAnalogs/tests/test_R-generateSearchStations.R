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

# This script tests RAnEn::generateSearchStations

library(RAnEn)

dist <- function(index1, index2) {
  return(sqrt((xs[index1] - xs[index2])^2 +
                (ys[index1] - ys[index2])^2))
}

# This is just a macro function for this test
test.plot <- function(search.stations) {
  plot(xs, ys, pch = 16, col = 'grey')
  points(xs[station.i], ys[station.i],
         pch = 3, col = 'red', lwd = 2)
  points(xs[search.stations[station.i, ]],
         ys[search.stations[station.i, ]],
         pch = 1, col = 'green', lwd = 2, cex = 2)
  text(xs[search.stations[station.i, ]],
       ys[search.stations[station.i, ]],
       labels = 1:num.nearest)
}

# Generate random points
xs <- sample(1:100, 50)
ys <- sample(1:100, 50)
num.nearest <- 30
distance <- 25

# Test the functions with and without a distance threshold
search.stations <- generateSearchStations(xs, ys, num.nearest)
search.stations.distance <- generateSearchStations(xs, ys, num.nearest, distance)

if (interactive()) {
  # Select a random station to plot if in interatice mode
  station.i <- 25
  
  par(mfrow = c(1, 2))
  test.plot(search.stations)
  test.plot(search.stations.distance)
} else {
  cat("*** Please use interactive mode to see figures ***\n")
}

# Check results
for (row.index in 1:nrow(search.stations.distance)) {
  for (col.index in 1:ncol(search.stations.distance)) {
    
    search.index <- search.stations.distance[row.index, col.index]
    
    if (!is.na(search.index)) {
      stopifnot(dist(row.index, search.index) <= distance)      
    }
  }
}

cat("You survived the tests for RAnEn::generateSearchStations with distance threshold!\n")
