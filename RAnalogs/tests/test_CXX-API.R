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
# lastest version of C++ API development is consistent with the standard results. This
# serves as a way to avoid bugs.
#
# Datasets used in this test include:
# - Solar dataset
#
# If the test result is `TRUE` which suggests the results from the development
# version and the standard results are the same, the development version 
# theoretically should be free of bugs.
#

# Genearte NetCDF file
unlink('solar-forecasts.nc')
unlink('solar-observations.nc')
unlink('wind-forecasts.nc')
unlink('wind-observations.nc')
unlink('mapping.txt')
unlink('similarity.nc')
unlink('analogs_separate.nc')
source('data-to-NetCDF-wind.R')
source('data-to-NetCDF-solar.R')

##################################################################################
#                                 Test 1: Wind                                   #
##################################################################################
# Configuration
rm(list = ls())
load('test-Wind.Rdata')
exe1 <- '../../output/bin/analogGenerator'
exe2 <- '../../output/bin/similarityCalculator'
exe3 <- '../../output/bin/analogSelector'
test_forecasts <- 'wind-forecasts.nc'
search_forecasts <- 'wind-forecasts.nc'
search_observations <- 'wind-observations.nc'
output_analog <- 'analog.nc'
num_members <- members.size
verbose <- 1
test_start <- c(0, 0, test.start - 1, 0)
test_count <- c(dim(fc)[1], dim(fc)[2], test.end - test.start + 1, dim(fc)[4])
search_start <- c(0, 0, train.start - 1, 0)
search_count <- c(dim(fc)[1], dim(fc)[2], train.end - train.start + 1, dim(fc)[4])

command1 <- paste(exe1, ' --test-forecast-nc=', test_forecasts,
                  ' --search-forecast-nc=', search_forecasts,
                  ' --observation-nc=', search_observations,
                  ' --analog-nc=', output_analog,
                  ' --members=', num_members, ' --verbose=', verbose,
                  ' --test-start=', paste(test_start, collapse = ' '),
                  ' --test-count=', paste(test_count, collapse = ' '),
                  ' --search-start=', paste(search_start, collapse = ' '),
                  ' --search-count=', paste(search_count, collapse = ' '),
                  sep = '')

command2 <- paste(exe2, ' --test-forecast-nc=', test_forecasts,
                  ' --search-forecast-nc=', search_forecasts,
                  ' --observation-nc=', search_observations,
                  ' --similarity-nc=similarity.nc',
                  ' --mapping-txt=mapping.txt',
                  ' --test-start=', paste(test_start, collapse = ' '),
                  ' --test-count=', paste(test_count, collapse = ' '),
                  ' --search-start=', paste(search_start, collapse = ' '),
                  ' --search-count=', paste(search_count, collapse = ' '),
                  ' --verbose=', verbose,
                  sep = '')

command3 <- paste(exe3, ' --similarity-nc=similarity.nc',
                  ' --observation-nc=', search_observations,
                  ' --mapping-txt=mapping.txt',
                  ' --analog-nc=analogs_separate.nc',
                  ' --members=', num_members, ' --verbose=', verbose,
                  sep = '')

command4 <- paste('diff analogs_separate.nc', output_analog)

if (verbose > 2) {
  print('Commandline options are:')
  print(command1)
  print(command2)
  print(command3)
  print(command4)
}

# Calculation
unlink(output_analog)
ret <- try(system(command1))
ret <- try(system(command2)) + ret
ret <- try(system(command3)) + ret
ret <- try(system(command4)) + ret

# Validation
if (ret == 0) {
  
  nc <- nc_open('analog.nc')
  analogs.cxx <- ncvar_get(nc, "Analogs")[, , , , 1]
  nc_close(nc)
  
  if (!identical(analogs.cxx, analogs.java)) {
    stop("Error: C++ results are not identical to Java resultsf in wind test.")
  }
  
} else {
  stop("Error: wind test failed!")
}


##################################################################################
#                                 Test 2: Solar                                  #
##################################################################################
# Configuration
rm(list = ls())
load('test-Solar.Rdata')
exe <- '../../output/bin/analogGenerator'
exe2 <- '../../output/bin/similarityCalculator'
exe3 <- '../../output/bin/analogSelector'
test_forecasts <- 'solar-forecasts.nc'
search_forecasts <- 'solar-forecasts.nc'
search_observations <- 'solar-observations.nc'
output_analog <- 'analog.nc'
num_members <- members.size
verbose <- 1
test_start <- c(0, 0, test.start - 1, 0)
test_count <- c(dim(fc)[1], dim(fc)[2], test.end - test.start + 1, dim(fc)[4])
search_start <- c(0, 0, train.start - 1, 0)
search_count <- c(dim(fc)[1], dim(fc)[2], train.end - train.start + 1, dim(fc)[4])

command1 <- paste(exe, ' --test-forecast-nc=', test_forecasts,
                  ' --search-forecast-nc=', search_forecasts,
                  ' --observation-nc=', search_observations,
                  ' --analog-nc=', output_analog,
                  ' --members=', num_members, ' --verbose=', verbose,
                  ' --test-start=', paste(test_start, collapse = ' '),
                  ' --test-count=', paste(test_count, collapse = ' '),
                  ' --search-start=', paste(search_start, collapse = ' '),
                  ' --search-count=', paste(search_count, collapse = ' '),
                  sep = '')

command2 <- paste(exe2, ' --test-forecast-nc=', test_forecasts,
                  ' --search-forecast-nc=', search_forecasts,
                  ' --observation-nc=', search_observations,
                  ' --similarity-nc=similarity.nc',
                  ' --mapping-txt=mapping.txt',
                  ' --test-start=', paste(test_start, collapse = ' '),
                  ' --test-count=', paste(test_count, collapse = ' '),
                  ' --search-start=', paste(search_start, collapse = ' '),
                  ' --search-count=', paste(search_count, collapse = ' '),
                  ' --verbose=', verbose,
                  sep = '')

command3 <- paste(exe3, ' --similarity-nc=similarity.nc',
                  ' --observation-nc=', search_observations,
                  ' --mapping-txt=mapping.txt',
                  ' --analog-nc=analogs_separate.nc',
                  ' --members=', num_members, ' --verbose=', verbose,
                  sep = '')

command4 <- paste('diff analogs_separate.nc', output_analog)

if (verbose > 2) {
  print('Commandline options are:')
  print(command1)
  print(command2)
  print(command3)
  print(command4)
}

# Calculation
unlink(output_analog)
unlink('mapping.txt')
unlink('similarity.nc')
unlink('analogs_separate.nc')
ret <- try(system(command1))
ret <- try(system(command2)) + ret
ret <- try(system(command3)) + ret
ret <- try(system(command4)) + ret


# Validation
if (ret == 0) {
  
  nc <- nc_open('analog.nc')
  analogs.cxx <- ncvar_get(nc, "Analogs")[, , , 1]
  nc_close(nc)
  
  if (!identical(analogs.cxx[1, , ], analogs.java[1, , ])) {
    stop("Error: C++ results are not identical to Java results in solar test.")
  }
  
} else {
  stop("Error: Analog generator failed for solar test!")
}

unlink(output_analog)
unlink('solar-forecasts.nc')
unlink('solar-observations.nc')
unlink('wind-forecasts.nc')
unlink('wind-observations.nc')
unlink('mapping.txt')
unlink('similarity.nc')
unlink('analogs_separate.nc')

print("You survived the the Solar tests for C++ API!")
