library(RAnEn)

config <- new(Config)
config$save_similarity <- T
config$save_analogs <- F

forecasts <- generateForecastsTemplate()
forecasts$Data <- array(runif(2500), dim = c(2, 5, 50, 5))
forecasts$Times <- as.POSIXct(1:50, origin = '1970-01-01')
forecasts$FLTs <- array(0:4, dim = 5)

observations <- generateObservationsTemplate()
observations$Data <- array(runif(1500), dim = c(1, 5, 300))
observations$Times <- 1:300

# AnEn with no circulars
AnEn <- generateAnalogs(forecasts, observations, 41:50, 1:40, sx_config = config)

# AnEn with circular index
forecasts$ParameterCirculars <- 2
AnEn.1 <- generateAnalogs(forecasts, observations, 41:50, 1:40, sx_config = config)

# AnEn with circular names
forecasts$ParameterCirculars <- 'par2'
forecasts$ParameterNames <- c('par1', 'par2')
AnEn.2 <- generateAnalogs(forecasts, observations, 41:50, 1:40, sx_config = config)

# Check results
stopifnot(identical(AnEn.1, AnEn.2))
stopifnot(!identical(AnEn, AnEn.2))
stopifnot(!identical(AnEn.1, AnEn))

cat("You survived the tests for circular!\n")
