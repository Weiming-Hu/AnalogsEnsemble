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

# This script is to test fileAggregate for Analogs
#

library(abind)
library(ncdf4)

old.wd <- getwd()
setwd('../../apps/app_analogGenerator/example/')

unlink('analogs_ctn.nc')
unlink('analogs2.nc')
unlink('analogs1.nc')
unlink('analogs_c.nc')

system("../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc -v 3 --obs-start 0 0 0 --obs-count 1 5 80 --test-start 0 0 0 0 --test-count 5 5 3 8 --search-start 0 0 0 0 --search-count 5 5 9 8 --time-match-mode 1 --analog-nc analogs1.nc --members 5")
system("../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc  -v 3 --obs-start 0 5 0 --obs-count 1 10 80 --test-start 0 5 0 0 --test-count 5 10 3 8 --search-start 0 5 0 0 --search-count 5 10 9 8 --time-match-mode 1 --analog-nc analogs2.nc --members 5")
system('../../../output/bin/fileAggregate -t Analogs -i analogs1.nc analogs2.nc -o analogs_c.nc -v 3 -a 2')
system("../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc  -v 3 --obs-start 0 0 0 --obs-count 1 15 80 --test-start 0 0 0 0 --test-count 5 15 3 8 --search-start 0 0 0 0 --search-count 5 15 9 8 --time-match-mode 1 --analog-nc analogs-ctn.nc --members 5")
