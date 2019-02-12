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
unlink('sim_ctn.nc')

# Generate sims separate files
system('../../output/bin/similarityCalculator --test-forecast-nc ../../tests/Data/test_forecasts.nc --search-forecast-nc ../../tests/Data/test_forecasts.nc --observation-nc ../../tests/Data/test_observations.nc -v 1 --obs-start 0 0 0  --obs-count 1 13 80 --test-start 0 0 0 0 --test-count 5 13 4 8 --search-start 0 0 0 0 --search-count 5 13 4 8 --time-match-mode 1 --similarity-nc sim_1.nc')
system('../../output/bin/similarityCalculator --test-forecast-nc ../../tests/Data/test_forecasts.nc --search-forecast-nc ../../tests/Data/test_forecasts.nc --observation-nc ../../tests/Data/test_observations.nc -v 1 --obs-start 0 13 0  --obs-count 1 2 80 --test-start 0 13 0 0 --test-count 5 2 4 8 --search-start 0 13 0 0 --search-count 5 2 4 8 --time-match-mode 1 --similarity-nc sim_2.nc')
system('../../output/bin/similarityCalculator --test-forecast-nc ../../tests/Data/test_forecasts.nc --search-forecast-nc ../../tests/Data/test_forecasts.nc --observation-nc ../../tests/Data/test_observations.nc -v 1 --obs-start 0 0 0  --obs-count 1 15 80 --test-start 0 0 0 0 --test-count 5 15 4 8 --search-start 0 0 0 0 --search-count 5 15 4 8 --time-match-mode 1 --similarity-nc sim_ctn.nc')

# Combine sims
system('../../output/bin/fileAggregate -t Similarity -i sim_1.nc sim_2.nc -o sim_c.nc -v 2 -a 0')

# Names to compare
names <- c('Xs', 'Ys', 'Times', 'FLTs', 'SearchXs', 'SearchYs', 'SearchTimes', 'SimilarityMatrices')

# Combine sims in R
nc1 <- nc_open('sim_ctn.nc')
nc2 <- nc_open('sim_c.nc')

for (name in names) {
  simsr <- ncvar_get(nc1, name)
  simsc <- ncvar_get(nc2, name)
  
  if (sum(abs(simsc - simsr), na.rm = T) != 0) {
    stop("Something is wrong for aggregating similarity matrices along entry times (", name, ")!")
  }
}

nc_close(nc2)
nc_close(nc1)

# if (system('diff sim_c.nc sim_ctn.nc', intern = FALSE)) {
#   stop("File aggregate does not generate same similarity matrices")
# }

cat("You passed the test for aggregating similarity matrices along stations!\n")

unlink('sim_1.nc')
unlink('sim_2.nc')
unlink('sim_c.nc')
unlink('sim_ctn.nc')

# Generate sims separate files
system('../../output/bin/similarityCalculator --test-forecast-nc ../../tests/Data/test_forecasts.nc --search-forecast-nc ../../tests/Data/test_forecasts.nc --observation-nc ../../tests/Data/test_observations.nc -v 1 --obs-start 0 0 0  --obs-count 1 20 80 --test-start 0 0 0 0 --test-count 5 20 10 3 --search-start 0 0 0 0 --search-count 5 20 4 3 --time-match-mode 1 --similarity-nc sim_1.nc')
system('../../output/bin/similarityCalculator --test-forecast-nc ../../tests/Data/test_forecasts.nc --search-forecast-nc ../../tests/Data/test_forecasts.nc --observation-nc ../../tests/Data/test_observations.nc -v 1 --obs-start 0 0 0  --obs-count 1 20 80 --test-start 0 0 0 3 --test-count 5 20 10 4 --search-start 0 0 0 3 --search-count 5 20 4 4 --time-match-mode 1 --similarity-nc sim_2.nc')
system('../../output/bin/similarityCalculator --test-forecast-nc ../../tests/Data/test_forecasts.nc --search-forecast-nc ../../tests/Data/test_forecasts.nc --observation-nc ../../tests/Data/test_observations.nc -v 1 --obs-start 0 0 0  --obs-count 1 20 80 --test-start 0 0 0 0 --test-count 5 20 10 7 --search-start 0 0 0 0 --search-count 5 20 4 7 --time-match-mode 1 --similarity-nc sim_ctn.nc')

# Combine sims
system('../../output/bin/fileAggregate -t Similarity -i sim_1.nc sim_2.nc -o sim_c.nc -v 2 -a 2')


# Names to compare
names <- c('Xs', 'Ys', 'Times', 'FLTs', 'SearchXs', 'SearchYs', 'SearchTimes', 'SimilarityMatrices')

# Combine sims in R
nc1 <- nc_open('sim_ctn.nc')
nc2 <- nc_open('sim_c.nc')

for (name in names) {
  simsr <- ncvar_get(nc1, name)
  simsc <- ncvar_get(nc2, name)
  
  if (sum(abs(simsc - simsr), na.rm = T) != 0) {
    stop("Something is wrong for aggregating similarity matrices (", name, ")!")
  }
}

nc_close(nc2)
nc_close(nc1)

# if (system('diff sim_c.nc sim_ctn.nc', intern = FALSE)) {
#   stop("File aggregate does not generate same similarity matrices")
# }

cat("You passed the test for aggregating similarity matrices along FLTs!\n")

unlink('sim_1.nc')
unlink('sim_2.nc')
unlink('sim_c.nc')
unlink('sim_ctn.nc')
