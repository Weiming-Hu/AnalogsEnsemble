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

# This script is to test fileAggregate and standardDeviationCalculator with indices
#

library(abind)
library(ncdf4)

source('data-generate-part-forecast-files.R')

# Generate sds separate files
exe <- '../../output/bin/standardDeviationCalculator'
files_in <- list.files(path = '.', pattern = "forecasts-part-", full.names = T)
file_out1 <- 'sds1.nc'
file_out2 <- 'sds2.nc'
verbose <- 2

start <- c()
count <- c()
for (file_in in files_in) {
  nc <- nc_open(file_in)
  count <- c(count, num_parameters-1, num_stations-3,
             length(ncvar_get(nc, "Times")), num_flts)
  start <- c(start, 1, 1, 0, 0)
  nc_close(nc)
}

command <- paste(exe, '-i', paste(files_in, collapse = ' '),
                 '--start', paste(start, collapse = ' '),
                 '--count', paste(count, collapse = ' '),
                 '-v', verbose, '-o')

unlink(file_out1)
unlink(file_out2)

system(paste(command, file_out1))
system(paste(command, file_out2))

# Combine forecasts in R
nc <- nc_open(files_in[1])
fcsts <- ncvar_get(nc, 'Data', start = start[1:4] + 1, count = count[1:4])
nc_close(nc)

for (i in 2:length(files_in)) {
  nc <- nc_open(files_in[i])
  fcsts.part <- ncvar_get(nc, 'Data', start = start[((i-1)*4+1):(i*4)] + 1,
                          count = count[((i-1)*4+1):(i*4)])
  nc_close(nc)
  fcsts <- abind(fcsts, fcsts.part, along = 3)
}

sds.r <- apply(fcsts, c(1, 2, 4), sd, na.rm = T)

# Check the separate files are correct with R results
nc <- nc_open(file_out1)
sds.c <- ncvar_get(nc, "StandardDeviation");
nc_close(nc)

dif <- abs(sds.r - sds.c)
dif <- dif[dif < 1e+10]
if (sum(dif, na.rm = T) < 1e-8) {
  cat("You passed the test for partial computatin of standard deviation!\n")
} else {
  stop("Something is wrong for partial computation of standard deviation!")
}

# Aggregate sd files
exe <- '../../output/bin/fileAggregate'
type <- 'StandardDeviation'
in_files <- paste(file_out1, file_out2)
out_file <- 'test_combine.nc'
along <- 2

command <- paste(exe, ' -t ', type,
                 ' -i ', in_files,
                 ' -o ', out_file, ' -a ', along-1,
                 ' -v ', verbose, sep = '')
unlink(out_file)
system(command)

# Combine sds with R
nc <- nc_open(file_out1)
sds1 <- ncvar_get(nc, 'StandardDeviation')
nc_close(nc)

nc <- nc_open(file_out2)
sds2 <- ncvar_get(nc, 'StandardDeviation')
nc_close(nc)

sds.r <- abind(sds1, sds2, along = along)

nc <- nc_open(out_file)
sds.c <- ncvar_get(nc, 'StandardDeviation')
nc_close(nc)

if (identical(as.vector(sds.c), as.vector(sds.r))) {
  cat("You passed the test for aggregating standard deviations!\n")
} else {
  stop("Something is wrong for aggregating standard deviations.")
}

unlink(file_out1)
unlink(file_out2)
unlink(out_file)
unlink(files_in)
