library(abind)
library(ncdf4)
library(RAnEn)

#############
# Forecasts #
#############

# nc <- nc_open('~/ExFat/PaperData_DeepAnalogs/NAM4DeepAnalogs/PA_NAM.nc')
# forecasts <- generateForecastsTemplate()
# forecasts$Xs <- ncvar_get(nc, 'Xs', collapse_degen = T)[485:494] - 360
# forecasts$Ys <- ncvar_get(nc, 'Ys', collapse_degen = T)[485:494]
# forecasts$StationNames <- 1:length(forecasts$Xs)
# forecasts$Times <- as.POSIXct(ncvar_get(nc, 'Times', collapse_degen = T)[2800:2880], origin = '1970-01-01', tz = 'UTC')
# forecasts$FLTs <- ncvar_get(nc, 'FLTs', collapse_degen = T)
# 
# pars_index <- c(8, 9, 10, 11)
# forecasts$ParameterNames <- ncvar_get(nc, 'ParameterNames')[pars_index]
# forecasts$Data <- ncvar_get(nc, 'Data', start = c(8, 485, 2500, 1), count = c(4, 10, 81, 4))
# 
# nc_close(nc)
# 
# rm(nc, pars_index)

################
# Observations #
################

# observations <- RAnEn::readObservations('~/ExFat/PaperData_DeepAnalogs_v2/Data/SURFRAD_PennState.nc')
# observations$Data <- observations$Data[3, , , drop = FALSE]
# observations$ParameterNames <- observations$ParameterNames[3]
# observations$StationNames <- 1:length(observations$StationNames)

# observations$Times <- observations$Times[41824:43824]
# observations$Data <- observations$Data[, , 41824:43824, drop = F]

# save(forecasts, observations, file = '~/github/AnalogsEnsemble/RAnalogs/tests/test-AnEnSSEMS.RData')

load('test-AnEnSSEMS.RData')

observations_dup <- observations
observations_dup$StationNames <- forecasts$StationNames
observations_dup$Xs <- forecasts$Xs
observations_dup$Ys <- forecasts$Ys
observations_dup$Data <- abind(lapply(1:length(forecasts$Xs), function(x) observations$Data), along=2)

closest_index <- which.min(((observations$Xs - forecasts$Xs) ^2 + (observations$Ys - forecasts$Ys) ^ 2))
observations_dup$Data[, -closest_index, ] <- NA

# maps::map('state', 'pennsylvania')
# maps::map('county', 'pennsylvania', add = T, col = 'lightgrey')
# points(forecasts$Xs, forecasts$Ys, pch = 19, cex = 0.5)
# points(forecasts$Xs[closest_index], forecasts$Ys[closest_index], pch = 1, cex = 2, col = 'green')
# points(observations$Xs, observations$Ys, pch = 16, cex = 1, col = 'red')

##########
# Config #
##########

config <- new(Config)
config$num_analogs <- 5
config$save_analogs <- TRUE
config$save_analogs_time_index <- TRUE
config$save_similarity <- TRUE
config$save_similarity_time_index <- TRUE
config$save_similarity_station_index <- TRUE
config$extend_observation <- FALSE
config$num_nearest <- 4
config$verbose <- 1

test_times <- forecasts$Times[70:81]
search_times <- forecasts$Times[1:69]

test <- function() {
  
  ###########
  # AnEnSSE #
  ###########
  
  AnEnSSE <- generateAnalogs(forecasts, observations_dup, test_times, search_times, config, 'SSE')
  
  #############
  # AnEnSSEMS #
  #############
  
  AnEnSSEMS <- generateAnalogs(forecasts, observations, test_times, search_times, config, 'SSE')
  
  #########
  # Check #
  #########
  
  stopifnot(all.equal(AnEnSSE$analogs[closest_index, , , , drop = F], AnEnSSEMS$analogs))
  stopifnot(all.equal(AnEnSSE$analogs_time_index[closest_index, , , , drop = F], AnEnSSEMS$analogs_time_index))
  stopifnot(all.equal(AnEnSSE$similarity[closest_index, , , , drop = F], AnEnSSEMS$similarity))
  stopifnot(all.equal(AnEnSSE$similarity_station_index[closest_index, , , , drop = F], AnEnSSEMS$similarity_station_index))
  stopifnot(all.equal(AnEnSSE$similarity_time_index[closest_index, , , , drop = F], AnEnSSEMS$similarity_time_index))
}

config$operation <- F
config$prevent_search_future <- T
test()

config$operation <- T
config$prevent_search_future <- T
test()

config$operation <- F
config$prevent_search_future <- F
test()

cat('You survived AnEnSSEMS tests!\n')
