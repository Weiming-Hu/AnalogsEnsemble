#!/usr/bin/env Rscript

# Read which file and FLT from command line arguments
args <- commandArgs(trailingOnly = T)
if (length(args) == 2) {
    i.file <- as.numeric(args[1])
    i.flt <- as.numeric(args[2])
} else {
    stop('Usage: Rscript 03-visualization.R <file index> <lead time index>')
}

# Load required libraries
library(maps)
library(ncdf4)
library(stringr)
library(raster)
library(RColorBrewer)

# This is the parameter id for wind speed
par.id <- 16

# Determine which file to read
analog.folder <- '/glade/u/home/wuh20/work/SEA2019/analogs/'
analog.file <- paste(analog.folder, 'day-', str_pad(i.file, 5, pad = 0), '.nc', sep = '')
cat('The analog file to read is', analog.file, '\n')

# Check whether this file exists
if (!file.exists(analog.file)) {
    stop('The analog file does not exist!')
}

# Read the analog file and some parameters
cat('Reading data from the analog file ...\n')
nc <- nc_open(analog.file)
count <- c(nc$dim$num_stations$len, 1, 1, nc$dim$num_members$len, 1)
analogs <- ncvar_get(nc, 'Analogs', start = c(1, 1, i.flt, 1, 1), count = count)
analogs.flt <- ncvar_get(nc, 'FLTs', start = i.flt, count = 1)
analogs.time <- as.POSIXct(ncvar_get(nc, 'Times'), origin = '1970-01-01', tz = 'UTC')
xs <- ncvar_get(nc, 'Xs') - 360
ys <- ncvar_get(nc, 'Ys')
nc_close(nc)

# Calculate the forecast time
forecast.time <- analogs.time + analogs.flt

# Calculate ensemble averages
analogs.mean <- rowMeans(analogs, na.rm = T)

# Create a raster template
cat("Creating visualization for", format(forecast.time, format = "%Y-%m-%d %H:%M:%S"), '...\n')
ext <- extent(cbind(xs, ys))
rast <- raster(ext, nrow = 428, ncol = 614, crs = CRS("+proj=longlat +datum=WGS84"))
rast <- rasterize(cbind(xs, ys), rast, analogs.mean, fun = mean)

file.name <- paste(format(forecast.time, format = "%Y-%m-%d-%H-%M-%S"), '.png', sep = '')
cat('Generating file', file.name, '\n')

png(file.name, width = 10, height = 8, unit = 'in', res = 300)
plot(rast, col = colorRampPalette(brewer.pal(11, 'Spectral')[11:1])(100),
     xlab = format(forecast.time, format = "%Y-%m-%d %H:%M:%S"))
map(add = T, col = 'lightgrey'); map('usa', add = T, col = 'grey')
dev.off()

cat('Done!\n')
