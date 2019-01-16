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

# This script is to test fileAggregate for SimilarityMatrices
#

library(abind)
library(ncdf4)

unlink('sim_1.nc')
unlink('sim_2.nc')
unlink('sim_c.nc')

# Generate sims separate files
system('../../output/bin/similarityCalculator --test-forecast-nc ../../tests/Data/test_forecasts.nc --search-forecast-nc ../../tests/Data/test_forecasts.nc --observation-nc ../../tests/Data/test_observations.nc -v 1 --test-start 0 0 8 0 --test-count 5 20 2 8 --search-start 0 0 0 0 --search-count 5 20 9 8 --time-match-mode 1 --similarity-nc sim_1.nc')
system('cp sim_1.nc sim_2.nc')

# Combine sims
system('../../output/bin/fileAggregate -t Similarity -i sim_1.nc sim_2.nc -o sim_c.nc -v 1 -a 3')

# Combine sims in R
nc <- nc_open('sim_1.nc')
sims1 <- ncvar_get(nc, 'SimilarityMatrices')
nc_close(nc)

nc <- nc_open('sim_2.nc')
sims2 <- ncvar_get(nc, 'SimilarityMatrices')
nc_close(nc)

nc <- nc_open('sim_c.nc')
simsc <- ncvar_get(nc, 'SimilarityMatrices')
nc_close(nc)

simsr <- abind(sims1, sims2, along = 2)

if (sum(abs(simsc - simsr)) == 0) {
  cat("You passed the test for aggregating similarity matrices!\n")
} else {
  stop("Something is wrong for aggregating similarity matrices")
}

unlink('sim_1.nc')
unlink('sim_2.nc')
unlink('sim_c.nc')
