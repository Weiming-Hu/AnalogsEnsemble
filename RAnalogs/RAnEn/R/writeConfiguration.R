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

#' RAnEn::writeConfiguration
#' 
#' RAnEn::writeConfiguration writes a Configuration class to multiple files, usually
#' observations and forecasts into NetCDF files and the rest configuration parameters
#' into a configuration file. The Configuration input should already be successfully
#' configured.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param config The successfully configured Configuration object.
#' @param obs.par.names The names of observation variables.
#' @param fcsts.par.names The names of forecast variables.
#' @param xs Xs of stations.
#' @param ys Ys of stations.
#' @param file.prefix File prefix for output files.
#' @param folder Output folder.
#' @param silent Whether to be silent.
#' 
#' @md
#' @export
writeConfiguration <- function(config, obs.par.names, fcsts.par.names,
                        xs, ys, file.prefix = '', folder = './',
                        silent = F) {
  stopifnot(require(ncdf4))
  
  # Sanity checks
  stopifnot(length(obs.par.names) == dim(config$search_observations)[1])
  stopifnot(length(fcsts.par.names) == dim(config$forecasts)[1])
  stopifnot(length(xs) == dim(config$forecasts)[2])
  stopifnot(length(ys) == dim(config$forecasts)[2])
  stopifnot(class(config) == 'Configuration')
  stopifnot(!config$advanced)
  if (config$mode != 'independentSearch') {
      stop("Currently only supporting writing independentSearch configuration.")
  }
  
  # Check whether config is successfully configured
  if (!silent) cat('Checking config ...\n')
  
  config <- formatConfiguration(config, config$verbose > 2)
  valid <- validateConfiguration(config)
  stopifnot(valid)
  
  global.attrs <- list(creation.time = format(Sys.time(), format = "%Y/%m/%d %H:%M:%S UTC%z"),
                       creator = paste('RAnEn', packageVersion("RAnEn")))
  
  file.obs <- paste(folder, file.prefix, 'observations.nc', sep = '')
  file.fcsts <- paste(folder, file.prefix, 'forecasts.nc', sep = '')
  file.cfg <- paste(folder, file.prefix, 'config.cfg', sep = '')
  file.analogs <- paste(folder, file.prefix, 'analogs.nc', sep = '')
  file.sims <- paste(folder, file.prefix, 'similarity.nc', sep = '')
  file.sds <- paste(folder, file.prefix, 'sds.nc', sep = '')
  file.mapping <- paste(folder, file.prefix, 'mapping.txt', sep = '')
  
  file.list <- c(file.obs, file.fcsts, file.cfg, file.analogs)
  
  if (config$preserve_mapping) {
    file.list <- c(file.list, file.mapping)
  }
  if (config$preserve_std) {
    file.list <- c(file.list, file.sds)
  }
  if (config$preserve_similarity) {
    file.list <- c(file.list, file.sims)
  }
  
  for (file in file.list) {
    if (file.exists(file)) {
      stop(paste('File exists', file))
    }
  }
  
  # Write observations from config
  if (!silent) cat('Writing', file.obs, '...\n')
  
  observations <- list(ParameterNames = obs.par.names,
                       Times = config$observation_times,
                       Data = config$search_observations,
                       Xs = xs, Ys = ys)
  writeNetCDF(file.type = 'Observations',
              obj = observations,
              file.out = file.obs,
              global.attrs = global.attrs,
              nchars.max = max(nchar(obs.par.names)))
  rm(observations)
  
  garbage <- gc(verbose = F, reset = T)
  
  # Write forecasts from config
  if (!silent) cat('Writing', file.fcsts, '...\n')
  
  forecasts <- list(ParameterNames = fcsts.par.names,
                    ParameterCirculars = fcsts.par.names[config$circulars],
                    Times = config$forecast_times,
                    Data = config$forecasts,
                    FLTs = config$flts,
                    Xs = xs, Ys = ys)
  writeNetCDF(file.type = 'Forecasts',
              obj = forecasts,
              file.out = file.fcsts,
              global.attrs = global.attrs,
              nchars.max = max(nchar(fcsts.par.names)))
  rm(forecasts)
  
  garbage <- gc(verbose = F, reset = T)
  
  # Write cfg file from config
  if (!silent) cat('Writing', file.cfg, '...\n')
  
  test.times.index <- sapply(config$test_times_compare, function(x, v) {
    which(x == v)},  v = config$forecast_times) - 1
  search.times.index <- sapply(config$search_times_compare, function(x, v) {
    which(x == v)},  v = config$forecast_times) - 1
  
  cfg.lines <- c(
    paste("# Author:", global.attrs$creator),
    paste("# Time of creation:", global.attrs$creation.time),
    '\n',
    paste("test-forecast-nc =", file.fcsts),
    paste("search-forecast-nc =", file.fcsts),
    paste("observation-nc =", file.obs),
    paste("analog-nc =", file.analogs))
  
  if (config$preserve_mapping) {
    cfg.lines <- c(cfg.lines, paste("mapping-txt =", file.mapping))
  }
  
  if (config$preserve_std) {
    cfg.lines <- c(cfg.lines, paste("sds-nc =", file.sds))
  }
  
  if (config$preserve_similarity) {
    cfg.lines <- c(cfg.lines, paste("similarity-nc =", file.sims))
  }
  
  cfg.lines <- c(cfg.lines, '\n',
                 paste("observation-id =", config$observation_id - 1),
                 paste("members =", config$num_members),
                 paste("quick =", as.numeric(config$quick)),
                 paste("extend-obs=", as.numeric(config$extend_observations)),
                 paste("operational =", as.numeric(config$operational)),
                 paste("time-match-mode =", config$time_match_mode),
                 paste("max-par-nan =", config$max_par_nan),
                 paste("max-flt-nan = ", config$max_flt_nan),
                 paste("max-num-sims =", config$max_num_sims),
                 paste("window-half-size =", config$FLT_radius),
                 paste("verbose =", config$verbose),
                 '\n',
                 paste("test-times-index =", test.times.index),
                 paste("search-times-index =", search.times.index))
  
  if (identical(NULL, names(config$weights))) {
    cfg.lines <- c(cfg.lines, '\n', paste("weights =", config$weights))
  } else {
    cfg.lines <- c(cfg.lines, '\n', as.vector(t(cbind(
      paste("#", names(config$weights)),
      paste("weights =", config$weights)))))
  }
  
  con <- file(file.cfg, 'w')
  writeLines(cfg.lines, con = con)
  close(con)
  
  if (!silent) cat('Done (writeConfiguration)!\n')
}
