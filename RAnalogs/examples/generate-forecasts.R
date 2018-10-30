# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <weiming@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

# This script generates an R data file with forecasts
library(ncdf4)
library(maps)

# Specify where the raw data are
data.folder <- '/media/wuh20/ExFat-Hu/Data/forecasts_reformat'
data.files <- list.files(path = data.folder, pattern = "*.nc", full.names = T)

# Read meta information from the first files
# It is assumed that all files have the same meta information except for times
#
nc <- nc_open(data.files[1])

par.names <- ncvar_get(nc, 'ParameterNames')
circular.pars <- ncvar_get(nc, 'ParameterCirculars')
flts <- ncvar_get(nc, 'FLTs')
num.pars <- length(par.names)
num.flts <- length(flts)

xs <- ncvar_get(nc, 'Xs')
ys <- ncvar_get(nc, 'Ys')
nc_close(nc)

# Specify the center station row and col and the radius
i.row <- 225
i.col <- 500
radius <- 5

# Determine the indices for these stations
# For NAM models, there are 428 rows and 614 columns in total
# Reference: http://www.nco.ncep.noaa.gov/pmb/docs/on388/tableb.html#GRID218
#
grid <- expand.grid(1:614, 1:428)
indices <- which(grid[, 1] <= i.col + radius & grid[, 1] >= i.col - radius &
                   grid[, 2] >= i.row - radius & grid[, 2] <= i.row + radius)
map('state', col = 'grey'); map('usa', add = T)
points(xs[indices] - 360, ys[indices], pch = 16, cex = 0.2)

num.stations <- length(indices)

data.list <- list()
times <- c()

for (i.file in 1:length(data.files)) {
  print(paste('Working on file ', i.file, ' out of ', length(data.files), sep = ''))
  
  data.file <- data.files[i.file]
  nc <- nc_open(data.file)
  
  times.file <- ncvar_get(nc, 'Times')
  times <- c(times, times.file)
  num.times <- length(times.file)
  
  file.list <- list()
  
  for (i in 1:length(indices)) {
    print(paste('-- Working on index ', i, ' out of ', length(indices), sep = ''))
    file.list[[i]] <- ncvar_get(nc, 'Data', start = c(1, indices[i], 1, 1), count = c(num.pars, 1, num.times, num.flts))
  }
  
  data.list[[i.file]] <- file.list
  nc_close(nc)
}

forecast.times <- as.POSIXct(times, origin = '1970-01-01', tz = 'UTC')
num.times <- length(forecast.times)

# This variable is used to append data to the end of the data
recorded.days <- 0
forecasts <- array(data = NA, dim = c(num.pars, num.stations, num.times, num.flts))
for (i in 1:length(data.list)) {
  for (j in 1:length(data.list[[i]])) {
    forecasts[, j, (recorded.days + 1):(recorded.days + dim(data.list[[i]][[j]])[2]), ] <- data.list[[i]][[j]]
  }
  recorded.days <- recorded.days + dim(data.list[[i]][[1]])[2]
}

# Make sure that there are same number of time values in forecast.times and forecasts
assertthat::are_equal(length(forecast.times), recorded.days)

# Reduce the amount of data
forecasts <- forecasts[, , , 1:36, drop = F]
flts <- flts[1:36]
forecast.xs <- xs[indices]
forecast.ys <- ys[indices]

forecast.indices <- indices
forecast.par.names <- par.names
forecast.circular.pars <- circular.pars

# Save data
save(forecasts, forecast.times, forecast.xs, forecast.ys, forecast.indices,
     forecast.par.names, forecast.circular.pars, flts, file = 'forecasts.RData')
