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

#' RAnEn::writeConfig
#' 
#' RAnEn::writeConfig writes a Configuration class to multiple files, usually
#' observations and forecasts into NetCDF files and the rest configuration parameters
#' into a configuration file. The Configuration input should already be successfully
#' configured.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param config The successfully configured Configuration object.
#' @param file The output file name
#' 
#' @md
#' @export
writeConfig <- function(config, file, overwrite = F) {
  
  # Sanity checks
  stopifnot(inherits(config, 'Rcpp_Config'))
  
  if (file.exists(file)) {
    if (overwrite) {
      file.remove(file)
    } else {
      stop(paste('File exists. Use overwrite = T'))
    }
  }
  
  pairs <- new(Config)$getNames()
  
  # Write cfg file from config
  cfg.lines <- c(
    paste("# Author: RAnEn", packageVersion('RAnEn'), "from GEOlab, Penn State"),
    paste("# Time of creation:", Sys.time()), '',
    paste(pairs$`_NUM_ANALOGS`, "=", config[[pairs$`_NUM_ANALOGS`]]),
    paste(pairs$`_NUM_SIMS`, "=", config[[pairs$`_NUM_SIMS`]]),
    paste(pairs$`_OBS_ID`, "=", ifelse(config[[pairs$`_OBS_ID`]] == 0, yes = 0, no = config[[pairs$`_OBS_ID`]] - 1)),
    paste(pairs$`_NUM_PAR_NA`, "=", config[[pairs$`_NUM_PAR_NA`]]),
    paste(pairs$`_NUM_FLT_NA`, "=", config[[pairs$`_NUM_FLT_NA`]]),
    paste(pairs$`_FLT_RADIUS`, "=", config[[pairs$`_FLT_RADIUS`]]),
    paste(pairs$`_NUM_NEAREST`, "=", config[[pairs$`_NUM_NEAREST`]]),
    paste(pairs$`_DISTANCE`, "=", ifelse(is.na(config[[pairs$`_DISTANCE`]]), yes = -1, no = config[[pairs$`_DISTANCE`]])),
    paste(pairs$`_EXTEND_OBS`, "=", as.numeric(config[[pairs$`_EXTEND_OBS`]])),
    paste(pairs$`_OPERATION`, "=", as.numeric(config[[pairs$`_OPERATION`]])),
    paste(pairs$`_PREVENT_SEARCH_FUTURE`, "=", as.numeric(config[[pairs$`_PREVENT_SEARCH_FUTURE`]])),
    paste(pairs$`_SAVE_ANALOGS`, "=", as.numeric(config[[pairs$`_SAVE_ANALOGS`]])),
    paste(pairs$`_SAVE_ANALOGS_TIME_IND`, "=", as.numeric(config[[pairs$`_SAVE_ANALOGS_TIME_IND`]])),
    paste(pairs$`_SAVE_SIMS`, "=", as.numeric(config[[pairs$`_SAVE_SIMS`]])),
    paste(pairs$`_SAVE_SIMS_TIME_IND`, "=", as.numeric(config[[pairs$`_SAVE_SIMS_TIME_IND`]])),
    paste(pairs$`_SAVE_SIMS_STATION_IND`, "=", as.numeric(config[[pairs$`_SAVE_SIMS_STATION_IND`]])),
    paste(pairs$`_SAVE_SDS`, "=", as.numeric(config[[pairs$`_SAVE_SDS`]])),
    paste(pairs$`_SAVE_OBS_TIME_IND_TABLE`, "=", as.numeric(config[[pairs$`_SAVE_OBS_TIME_IND_TABLE`]])),
    paste(pairs$`_SAVE_SEARCH_STATIONS_IND`, "=", as.numeric(config[[pairs$`_SAVE_SEARCH_STATIONS_IND`]])),
    paste(pairs$`_QUICK`, "=", as.numeric(config[[pairs$`_QUICK`]])),
    paste(pairs$`_VERBOSE`, "=", config[[pairs$`_VERBOSE`]]))
  
  if (length(config$weights) != 0) {
    cfg.lines <- c(cfg.lines, '', paste("weights =", config$weights))
  }
  
  con <- file(file, 'w')
  writeLines(cfg.lines, con = con)
  close(con)
}
