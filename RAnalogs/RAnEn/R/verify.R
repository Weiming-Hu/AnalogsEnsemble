#  "`-''-/").___..--''"`-._
# (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
# (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#   _ ..`--'_..-_/  /--'_.' ,'
# (il),-''  (li),'  ((!.-'
#
#
# Author: Weiming Hu <weiming@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::verify
#' 
#' RAnEn::verify is the caller function to call various verification metrics. 
#' This is convenient when you want to carry out multiple verification at
#' once.
#' 
#' @param metrics A vector of verification metrics that should be carried out. Please
#' use \code{\link{showVerificationMetrics}} to check the supported metrics. 
#' @param verbose Whether to print detail information.
#' 
#' @return A list of verification results.
#' 
#' @md
#' @export
verify <- function(metrics, verbose = T, ...) {
  
  # Currently supported metrics are the followings
  supported.metrics <- c('ThreatScore', 'Brier', 'MAE', 'RMSE', 'CRMSE',
                         'Correlation', 'Bias', 'RankHist', 'Spread',
                         'SpreadSkill', 'Dispersion', 'CRPS')
  
  # List the required options for each verification metric
  args.required <- list(
    ThreatScore = c('anen.ver', 'obs.ver', 'threshold', 'ensemble.func'),
    Brier = c('anen.ver', 'obs.ver', 'threshold', 'ensemble.func'),
    MAE = c('anen.ver', 'obs.ver'),
    RMSE = c('anen.ver', 'obs.ver'),
    CRMSE = c('anen.ver', 'obs.ver'),
    Correlation = c('anen.ver', 'obs.ver'),
    Bias = c('anen.ver', 'obs.ver'),
    RankHist = c('anen.ver', 'obs.ver'),
    Spread = c('anen.ver'),
    SpreadSkill = c('anen.ver', 'obs.ver'),
    Dispersion = c('anen.ver', 'obs.ver'),
    CRPS = c('anen.ver', 'obs.ver'))
  
  # List the optional options for each verification metric
  args.optional <- list(
    ThreatScore = c(),
    Brier = c('baseline'),
    MAE = c('boot', 'R', 'na.rm', 'parallel'),
    RMSE = c('boot', 'R', 'na.rm', 'parallel'),
    CRMSE = c('boot', 'R', 'na.rm', 'parallel'),
    Correlation = c('boot', 'R', 'na.rm', 'parallel'),
    Bias = c('boot', 'R', 'na.rm', 'parallel'),
    RankHist = c('show.progress'),
    Spread = c('na.rm', 'parallel'),
    SpreadSkill = c('boot', 'R', 'na.rm', 'intervals'),
    Dispersion = c('boot', 'R', 'na.rm'),
    CRPS = c('boot', 'R', 'na.rm', 'int.step'))
  
  # Expand the input arguments
  args.all <- match.call(expand.dots = T)
  
  # Get the names of arguments
  named.args <- names(args.all)[-1]
  
  # Sanity checks
  if ("" %in% named.args) {
    stop('Please give a name to each variable. Positional argument is forbidden!')
  }
  
  # Check whether all desired metrics are supported
  supported <- sapply(metrics, function(x) {x %in% supported.metrics})
  if (!all(supported)) {
    print(supported)
    stop('Unsupported metrics are found.')
  }
  rm(supported)
  
  # Check whether required arguments are specified
  error <- F
  for (metric in metrics) {
    for (arg.required in args.required[[metric]]) {
      if (!arg.required %in% named.args) {
        cat('Required argument', arg.required, 'is missing for the metric', metric, '.\n')
        error <- T
      }
    }
  }
  rm(metric, arg.required)
  
  # Check whether there are unused arguments
  accepted.arguments <- unique(c(
    'metrics', 'verbose', 
    unlist(args.required, use.names = F),
    unlist(args.optional, use.names = F)))
  
  if ('ensemble.func' %in% named.args) {
    arguments.func <- names(as.list(args('ensemble.func')))
    arguments.func <- arguments.func[-which(arguments.func == '')]
    accepted.arguments <- c(accepted.arguments, arguments.func)
    
    if (any(duplicated(accepted.arguments))) {
      stop('Arguments for the ensemble function are duplicated with verification functions.')
    }
  }
  
  for (arg.name in named.args) {
    if (!arg.name %in% accepted.arguments) {
      cat('Unused argument:', arg.name, '\n')
      error <- T
    }
  }
  rm(arg.name)
  
  # Housekeeping
  if (error) {
    stop('Requirement is not fulfilled.')
  }
  
  rm(error)
  
  if (verbose) {
    cat('The following verification will be carried out:',
        paste(metrics, collapse = ', '), '\n')
  }
  
  ret <- list()
  for (metric in metrics) {
    
    args.current <- list()
    for (arg in args.required[[metric]]) {
      args.current[[arg]] <- args.all[[arg]]
    }
    
    for (arg in args.optional[[metric]]) {
      args.current[[arg]] <- args.all[[arg]]
    }
    
    if ('ensemble.func' %in% names(args.current)) {
      for (arg in arguments.func) {
        args.current[[arg]] <- args.all[[arg]]
      }
    }
    
    if (verbose) cat('Verifying', metric, '[',
                     paste(names(args.current), collapse = ',')
                     ,']...\n')
    ret[[metric]] <- do.call(
      what = paste('verify', metric, sep = ''),
      args = args.current)
  }
  
  rm(args.all, args.required, args.optional, args.current)
  garbage <- gc(reset = T)
  cat('Done\n')
  return(ret)
}
