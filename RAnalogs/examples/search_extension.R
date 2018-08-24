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
# this is a demo to use the search space extension functionality in RAnEn

library(reshape2)
library(RAnEn)

# This is the column for station ID in metrics
COL_STATION <- 1

################
# Prepare Data #
################
# 1. create some synthetic data
# 2. and specify some parameters for AnEn computation
#

# these are the four dimensions for input forecasts
# and observations
#
num.parmeters <- 1
num.stations <- 2000
num.days <- 50
num.flts <- 1
members.size <- 5

search.times <- 1:num.days * 100
search.flts <- 1:num.flts
observation.times <- rep(search.times, each = num.flts) + num.flts

# create synthetic data
# values are generated from normal distributions
# with different maen and variance
#
forecasts <- array(data = rnorm(num.parmeters * num.stations
																* num.days * num.flts, mean=1000, sd=10),
									 dim = c(num.parmeters, num.stations, num.days, num.flts))
observations <- array(data = forecasts[1, , , ] * 50,
											dim = c(1, num.stations, num.days * num.flts))

# To use search extension, we need to provide the x and y for each stations.
# Please make sure the lengths of xs and ys equal the number of stations.
# Here the xs and ys are randomly generated from a bag of integers.
#
xs <- as.numeric(sample.int(5*num.stations, num.stations))
ys <- as.numeric(sample.int(5*num.stations, num.stations))

# AnEn parameters
test.ID.start <- 42
test.ID.end <- 50
search.ID.start <- 1
search.ID.end <- 40
stations.ID <- c(30, 50, 70, 20)

config <- generateConfiguration('extendedSearch')
config$test_forecasts <- forecasts[, stations.ID, test.ID.start:test.ID.end, , drop = F]
config$search_forecasts <- forecasts[, , search.ID.start:search.ID.end, , drop = F]
config$search_times <- search.times[search.ID.start:search.ID.end]
config$search_flts <- search.flts
config$search_observations <- observations
config$observation_times <- observation.times
config$num_members <- members.size
config$weights <- rep(1, num.parmeters)
config$test_stations_x <- xs[stations.ID]
config$test_stations_y <- ys[stations.ID]
config$search_stations_x <- xs
config$search_stations_y <- ys
config$max_num_search_stations <- 25
config$preserve_mapping <- T
config$verbose <- 3

# save search stations in the output
config$preserve_search_stations <- T

# save metrics in the output
config$preserve_similarity <- T

# RAnEn provide 3 functions for finding search stations:
# 1. find a number of nearest neighbor stations;
# 2. find the stations within a certain distance;
# 3. find a number of nearest neighbot stations, and then
#    exclude any stations that are not within a certain 
#    distance
#
# you can comment/uncomment either one below to 
# play around with them
#
config$num_nearest <- 20
config$distance <- 1000


###################
# Compute Analogs #
###################
# 1. compute AnEn with search extension feature
# 2. compute AnEn without search extension feature
#

# w/ search extension
AnEn <- generateAnalogs(config)


# w/o search extension
config.ind <- generateConfiguration('independentSearch')
config.ind$test_forecasts <- forecasts[, stations.ID, test.ID.start:test.ID.end, , drop = F]
config.ind$search_forecasts <- forecasts[, stations.ID, search.ID.start:search.ID.end, , drop = F]
config.ind$search_times <- search.times[search.ID.start:search.ID.end]
config.ind$search_flts <- search.flts
config.ind$search_observations <- observations[, stations.ID, , drop = F]
config.ind$observation_times <- observation.times
config.ind$num_members <- members.size
config.ind$weights <- rep(1, num.parmeters)
config.ind$preserve_similarity <- T
config.ind$preserve_mapping <- T

AnEn.ind <- generateAnalogs(config.ind)

###################
# Analyze Results #
###################
# 1. get text-based information for the results
# 2. analyze the search extension feature
# 3. compare the errors and metrics with/without
#    serach extension feature
#

# you can use summary or print to get a text-based
# report of the output
#
class(AnEn)
print(AnEn)
summary(AnEn)

# get the members from AnEn list
analogs <- AnEn$analogs
search.stations <- AnEn$searchStations
metrics <- AnEn$similarity

if (T) {
	# geographical locations of main, search stations,
	# and the stations from which the values are taken
	#
	
	# for plotting legend outside of the plot region
	# turn off the clipping
	#
	#par(xpd=TRUE)
	
	cex <- .8
	offset <- 10
	label.size <- .8
	title <- 'Search Extension Demo'
	
	# get the selected stations from ensemble members
	main.station.index <- 2
	day.index <- 1
	flt.index <- 1
	selected.stations <- unique(
		analogs[main.station.index, day.index, flt.index, , COL_STATION])
	main.station.ID <- stations.ID[main.station.index]
	
	search.stations.ID <- search.stations[,main.station.index]
	
	# plot different types of stations together
	par(mfrow = c(1, 1))
	plot(xs, ys, main = title, pch = 19, cex = cex, col = 'grey')
	points(xs[search.stations.ID], ys[search.stations.ID],
				 pch = 19, cex = cex, col = 'green')
	points(xs[main.station.ID], ys[main.station.ID],
				 pch = 19, cex = cex/2, col = 'red')
	points(xs[selected.stations], ys[selected.stations],
				 cex = cex*1.6, col = 'red')
	
	
	if (config$distance > 0) {
		coords <- generateCircleCoords(
			x = xs[main.station.ID], y = ys[main.station.ID],
			radius = config$distance, np = 100)
		
		points(coords, cex = .1, pch = 19)
	}
	
	legend('top', legend = c('general station', 'search station',
													 'main station', 'selected station'),
				 pch = c(19, 19, 19 , 1), horiz = T,
				 col = c('grey', 'green', 'red', 'red'))
}

if (T) {
	# compare the results from method with and without
	# search extension
	#
	main.station.index <- 1
	day.index <- 1
	flt <- 1
	
	obs.lwd <- 2
	obs.col <- 'red'
	
	metrics.se <- metrics[main.station.index, day.index, flt, ,]
	metrics.wose <- AnEn.ind$similarity[main.station.index, day.index, flt, ,]
	compare <- cbind(metrics.se[1:members.size, ],
									 metrics.wose[1:members.size, ])
	colnames(compare) <- c('station_se', 'day_se', 'metric_se',
												 'station_wose', 'day_wos', 'metric_wose')
	print(compare)
	
	# prepare data to plot
	all.days.value.se <- t(analogs[
		main.station.index, , flt, , 3])
	all.days.value.wose <- t(AnEn.ind$analogs[
		main.station.index, , flt, , 3])
	obs <- observations[1, stations.ID[main.station.index],
											test.ID.start:test.ID.end[day.index]]
	
	ylim <- range(cbind(all.days.value.se, all.days.value.wose), na.rm = T)
	
	x0s <- 1:(test.ID.end - test.ID.start + 1) - .5
	x1s <- 1:(test.ID.end - test.ID.start + 1) + .5
	
	colnames(all.days.value.se) <- test.ID.start:test.ID.end
	colnames(all.days.value.wose) <- test.ID.start:test.ID.end
	
	# compare the distributions of the two methods
	par(mfrow = c(1, 2), mar = c(5,2,3,1))
	boxplot(all.days.value.se, xlab = 'Test Day', ylim = ylim,
					main = 'Extended Search Method')
	segments(x0 = x0s, y0 = obs, x1 = x1s,
					 col = obs.col, lty = 'dotted', lwd = obs.lwd)
	
	boxplot(all.days.value.wose, xlab = 'Test Day',
					ylim = ylim, yaxt = 'n', main = 'Indenpendent Search Method')
	segments(x0 = x0s, y0 = obs, x1 = x1s,
					 col = obs.col, lty = 'dotted', lwd = obs.lwd)
}
