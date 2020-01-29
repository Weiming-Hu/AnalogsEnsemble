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
unlink('analogs_ctn_small.nc')
unlink('analogs_part_c.nc')

system("../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc -v 2 --obs-start 0 0 0 --obs-count 1 5 80 --test-start 0 0 0 0 --test-count 5 5 3 8 --search-start 0 0 0 0 --search-count 5 5 9 8 --time-match-mode 0 --analog-nc analogs1.nc --members 5")
system("../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc  -v 2 --obs-start 0 5 0 --obs-count 1 10 80 --test-start 0 5 0 0 --test-count 5 10 3 8 --search-start 0 5 0 0 --search-count 5 10 9 8 --time-match-mode 0 --analog-nc analogs2.nc --members 5")
system('../../../output/bin/fileAggregate -t Analogs -i analogs1.nc analogs2.nc -o analogs_c.nc -v 2 -a 0')
system("../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc  -v 2 --obs-start 0 0 0 --obs-count 1 15 80 --test-start 0 0 0 0 --test-count 5 15 3 8 --search-start 0 0 0 0 --search-count 5 15 9 8 --time-match-mode 0 --analog-nc analogs_ctn.nc --members 5")
system("../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc  -v 2 --obs-start 0 0 0 --obs-count 1 15 80 --test-start 0 0 0 0 --test-count 5 15 3 8 --search-start 0 0 0 0 --search-count 5 15 9 8 --time-match-mode 0 --analog-nc analogs_ctn_small.nc --members 5 --max-num-sims 5")

system('../../../output/bin/fileAggregate -t Analogs -i analogs1.nc analogs2.nc -o analogs_part_c.nc -v 2 -a 0 --start 1 1 0 0 0 1 1 0 0 0 --count 3 2 8 5 3 9 2 8 5 3')

nc <- nc_open('analogs_part_c.nc')
analogs.pc <- ncvar_get(nc, 'Analogs')
times.pc <- ncvar_get(nc, 'Times')
flts.pc <- ncvar_get(nc, 'FLTs')
nc_close(nc)

nc <- nc_open('analogs_ctn.nc')
analogs.ctn <- ncvar_get(nc, 'Analogs')
member.times.ctn <- ncvar_get(nc, "MemberTimes")
member.xs.ctn <- ncvar_get(nc, "MemberXs")
nc_close(nc)

nc <- nc_open('analogs_c.nc')
analogs.c <- ncvar_get(nc, 'Analogs')
times.c <- ncvar_get(nc, 'Times')
flts.c <- ncvar_get(nc, 'FLTs')
member.times.c <- ncvar_get(nc, "MemberTimes")
member.xs.c <- ncvar_get(nc, "MemberXs")
nc_close(nc)

if (!identical(analogs.c, analogs.ctn) && 
    !identical(member.times.c, member.times.ctn) && 
    !identical(member.xs.ctn, member.xs.c)) {
  stop("File aggregate does not generate same analogs.")
}

if (!identical(analogs.c[c(-1, -5, -6), -1, , , ], analogs.pc) &&
    !identical(times.pc, times.c[-1]) &&
    !identical(flts.c, flts.pc)) {
  stop("File aggregate does not generate same analogs for partial reading.")
}


nc <- nc_open('analogs_ctn_small.nc')
analogs.small <- ncvar_get(nc, 'Analogs')
member.times.small <- ncvar_get(nc, "MemberTimes")
member.xs.small <- ncvar_get(nc, "MemberXs")
nc_close(nc)

if (!identical(analogs.c, analogs.small) && 
    !identical(member.times.c, member.times.small) && 
    !identical(member.xs.ctn, member.xs.small)) {
  stop("File aggregate does not generate same analogs.")
}

cat("Pass the test of aggregating analogs along test stations.\n")

unlink('analogs_ctn.nc')
unlink('analogs2.nc')
unlink('analogs1.nc')
unlink('analogs_c.nc')
unlink('analogs_part_c.nc')

system("../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc -v 2 --obs-start 0 0 0 --obs-count 1 15 80 --test-start 0 0 0 0 --test-count 5 15 3 3 --search-start 0 0 0 0 --search-count 5 15 9 3 --time-match-mode 1 --analog-nc analogs1.nc --members 5")
system("../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc  -v 2 --obs-start 0 0 0 --obs-count 1 15 80 --test-start 0 0 0 3 --test-count 5 15 3 4 --search-start 0 0 0 3 --search-count 5 15 9 4 --time-match-mode 1 --analog-nc analogs2.nc --members 5")
system('../../../output/bin/fileAggregate -t Analogs -i analogs1.nc analogs2.nc -o analogs_c.nc -v 2 -a 2')
system("../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc  -v 2 --obs-start 0 0 0 --obs-count 1 15 80 --test-start 0 0 0 0 --test-count 5 15 3 7 --search-start 0 0 0 0 --search-count 5 15 9 7 --time-match-mode 1 --analog-nc analogs_ctn.nc --members 5")
system('../../../output/bin/fileAggregate -t Analogs -i analogs1.nc analogs2.nc -o analogs_part_c.nc -v 2 -a 2 --start 0 1 1 0 0 0 1 2 0 0 --count 15 2 2 5 3 15 2 2 5 3')

nc <- nc_open('analogs_part_c.nc')
analogs.pc <- ncvar_get(nc, 'Analogs')
times.pc <- ncvar_get(nc, 'Times')
flts.pc <- ncvar_get(nc, 'FLTs')
nc_close(nc)

nc <- nc_open('analogs_ctn.nc')
analogs.ctn <- ncvar_get(nc, 'Analogs')
member.times.ctn <- ncvar_get(nc, "MemberTimes")
member.xs.ctn <- ncvar_get(nc, "MemberXs")
nc_close(nc)

nc <- nc_open('analogs_c.nc')
analogs.c <- ncvar_get(nc, 'Analogs')
times.c <- ncvar_get(nc, 'Times')
flts.c <- ncvar_get(nc, 'FLTs')
member.times.c <- ncvar_get(nc, "MemberTimes")
member.xs.c <- ncvar_get(nc, "MemberXs")
nc_close(nc)

if (!identical(analogs.c[, -1, c(-1, -4, -5), , ], analogs.pc) &&
    !identical(times.pc, times.c[-1]) &&
    !identical(flts.c[c(-1, -4, -5)], flts.pc)) {
  stop("File aggregate does not generate same analogs for partial reading.")
}

if (!identical(analogs.c, analogs.ctn)) {
  stop("File aggregate does not generate same analogs.")
}

if (system('diff analogs_c.nc analogs_ctn.nc', intern = FALSE)) {
  stop("File aggregate does not generate same analogs.")
}

unlink('analogs_ctn.nc')
unlink('analogs2.nc')
unlink('analogs1.nc')
unlink('analogs_c.nc')
unlink('analogs_part_c.nc')

cat("Pass the test of aggregating analogs along FLTs.\n")

setwd(old.wd)
