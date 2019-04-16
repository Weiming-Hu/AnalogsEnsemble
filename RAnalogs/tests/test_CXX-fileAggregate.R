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
# lastest version of File Aggregate utility is correct.
#
# If the test result is `TRUE` which suggests the results from the development
# version and the standard results are the same. The development version 
# theoretically should be free of bugs.
#

####################################################################################
#                               Test Combining Forecasts                           #
####################################################################################

library(abind)
library(ncdf4)

rm(list = ls())
exe <- '../../output/bin/fileAggregate'
type <- 'Forecasts'
in_files <- '../../tests/Data/forecasts_part1.nc ../../tests/Data/forecasts_part2.nc'
out_file <- 'test_combine.nc'
verbose <- 1

for (along in c(0, 1, 2, 3)) {
  
  command <- paste(exe, ' -t ', type,
                   ' -i ', paste(in_files, collapse = ' '),
                   ' -o ', out_file, ' -a ', along,
                   ' -v ', verbose, sep = '')
  
  unlink(out_file)
  ret <- try(system(command))
  
  nc1 <- nc_open('../../tests/Data/forecasts_part1.nc')
  nc2 <- nc_open('../../tests/Data/forecasts_part2.nc')
  nc3 <- nc_open('test_combine.nc')
  
  data1 <- ncvar_get(nc1, 'Data')
  data2 <- ncvar_get(nc2, 'Data')
  data3 <- ncvar_get(nc3, 'Data')
  
  parameters1 <- ncvar_get(nc1, 'ParameterNames')
  parameters2 <- ncvar_get(nc2, 'ParameterNames')
  parameters3 <- ncvar_get(nc3, 'ParameterNames')
  
  times1 <- ncvar_get(nc1, 'Times')
  times2 <- ncvar_get(nc2, 'Times')
  times3 <- ncvar_get(nc3, 'Times')
  
  xs1 <- ncvar_get(nc1, 'Xs')
  xs2 <- ncvar_get(nc2, 'Xs')
  xs3 <- ncvar_get(nc3, 'Xs')
  
  flts1 <- ncvar_get(nc1, 'FLTs')
  flts2 <- ncvar_get(nc2, 'FLTs')
  flts3 <- ncvar_get(nc3, 'FLTs')
  
  nc_close(nc1)
  nc_close(nc2)
  nc_close(nc3)
  
  assertthat::assert_that(all(data3 == abind(data1, data2, along = along + 1), na.rm = T))
  
  if (along == 0) {
    assertthat::assert_that(all(times3 == times1))
    assertthat::assert_that(all(xs3 == xs1))
    assertthat::assert_that(all(flts3 == flts1))
    assertthat::assert_that(all(parameters3 == c(parameters1, parameters2)))
    
  } else if (along == 1) {
    assertthat::assert_that(all(parameters3 == parameters1))
    assertthat::assert_that(all(times3 == times1))
    assertthat::assert_that(all(flts3 == flts1))
    assertthat::assert_that(all(xs3 == c(xs1, xs2)))
    
  } else if (along == 2) {
    assertthat::assert_that(all(parameters3 == parameters1))
    assertthat::assert_that(all(xs3 == xs1))
    assertthat::assert_that(all(flts3 == flts1))
    assertthat::assert_that(all(times3 == c(times1, times2)))
    
  } else if (along == 3) {
    assertthat::assert_that(all(parameters3 == parameters1))
    assertthat::assert_that(all(xs3 == xs1))
    assertthat::assert_that(all(times3 == times1))
    assertthat::assert_that(all(flts3 == c(flts1, flts2)))
    
  } else {
    stop("Something wierd happened ...")
  }
  
  unlink(out_file) 
}

####################################################################################
#                               Test Combining Observations                        #
####################################################################################
rm(list = ls())

exe <- '../../output/bin/fileAggregate'
out_file <- 'test_combine.nc'
verbose <- 1
type <- 'Observations'
in_files <- '../../tests/Data/observations_part1.nc ../../tests/Data/observations_part2.nc'

for (along in c(0, 1, 2)) {
  command <- paste(exe, ' -t ', type,
                   ' -i ', paste(in_files, collapse = ' '),
                   ' -o ', out_file, ' -a ', along,
                   ' -v ', verbose, sep = '')
  
  unlink(out_file)
  ret <- try(system(command))
  
  nc1 <- nc_open('../../tests/Data/observations_part1.nc')
  nc2 <- nc_open('../../tests/Data/observations_part2.nc')
  nc3 <- nc_open('test_combine.nc')
  
  data1 <- ncvar_get(nc1, 'Data')
  data2 <- ncvar_get(nc2, 'Data')
  data3 <- ncvar_get(nc3, 'Data')
  
  parameters1 <- ncvar_get(nc1, 'ParameterNames')
  parameters2 <- ncvar_get(nc2, 'ParameterNames')
  parameters3 <- ncvar_get(nc3, 'ParameterNames')
  
  times1 <- ncvar_get(nc1, 'Times')
  times2 <- ncvar_get(nc2, 'Times')
  times3 <- ncvar_get(nc3, 'Times')
  
  xs1 <- ncvar_get(nc1, 'Xs')
  xs2 <- ncvar_get(nc2, 'Xs')
  xs3 <- ncvar_get(nc3, 'Xs')
  
  nc_close(nc1)
  nc_close(nc2)
  nc_close(nc3)
  
  assertthat::assert_that(all(data3 == abind(data1, data2, along = along + 1), na.rm = T))
  
  if (along == 0) {
    assertthat::assert_that(all(times3 == times1))
    assertthat::assert_that(all(xs3 == xs1))
    assertthat::assert_that(all(parameters3 == c(parameters1, parameters2)))
    
  } else if (along == 1) {
    assertthat::assert_that(all(parameters3 == parameters1))
    assertthat::assert_that(all(times3 == times1))
    assertthat::assert_that(all(xs3 == c(xs1, xs2)))
    
  } else if (along == 2) {
    assertthat::assert_that(all(parameters3 == parameters1))
    assertthat::assert_that(all(xs3 == xs1))
    assertthat::assert_that(all(times3 == c(times1, times2)))
    
  } else {
    stop("Something wierd happened ...")
  }
  
  unlink(out_file) 
}

cat("You survived the combining tests!\n")
