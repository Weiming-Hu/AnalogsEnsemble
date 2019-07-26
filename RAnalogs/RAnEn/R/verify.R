#  "`-''-/").___..--''"`-._
# (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
# (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#   _ ..`--'_..-_/  /--'_.' ,'
# (il),-''  (li),'  ((!.-'
#
#
# Author: Guido Cervone (cervone@psu.edu), Martina Calovi (mxc895@psu.edu), Laura Clemente-Harding (laura@psu.edu)
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::verify
#' 
#' RAnEn::verify is the caller function to call various verification methods.
#' This is convenient when you want to carry out multiple verification at
#' once.
#' 
#' @param methods A vector of verification methods that should be carried out. Please
#' use \code{\link{showVerificationMethods}} to check the supported methods.
#' @param verbose Whether to print detail information.
#' 
#' @return A list of verification results.
#' 
#' @md
#' @export
verify <- function(methods, verbose = T, ...) {
  
  # Currently supported methods are the followings
  supported.methods <- c('ThreatScore', 'Brier', 'MAE', 'RMSE', 'CRMSE',
                         'Correlation', 'Bias', 'RankHist', 'Spread',
                         'SpreadSkill', 'Dispersion', 'CRPS')
  
  # List the required options for each verification method
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
  
  # List the optional options for each verification method
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
  
  # Check whether all desired methods are supported
  supported <- sapply(methods, function(x) {x %in% supported.methods})
  if (!all(supported)) {
    print(supported)
    stop('Unsupported methods are found.')
  }
  rm(supported)
  
  # Check whether required arguments are specified
  error <- F
  for (method in methods) {
    for (arg.required in args.required[[method]]) {
      if (!arg.required %in% named.args) {
        cat('Required argument', arg.required, 'is missing for the method', method, '.\n')
        error <- T
      }
    }
  }
  rm(method, arg.required)
  
  # Check whether there are unused arguments
  accepted.arguments <- unique(c(
    'methods', 'verbose', 
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
        paste(methods, collapse = ', '), '\n')
  }
  
  ret <- list()
  for (method in methods) {
    
    args.current <- list()
    for (arg in args.required[[method]]) {
      args.current[[arg]] <- args.all[[arg]]
    }
    
    for (arg in args.optional[[method]]) {
      args.current[[arg]] <- args.all[[arg]]
    }
    
    if ('ensemble.func' %in% names(args.current)) {
      for (arg in arguments.func) {
        args.current[[arg]] <- args.all[[arg]]
      }
    }
    
    if (verbose) cat('Verifying', method, '[',
                     paste(names(args.current), collapse = ',')
                     ,']...\n')
    ret[[method]] <- do.call(
      what = paste('verify', method, sep = ''),
      args = args.current)
  }
  
  rm(args.all, args.required, args.optional, args.current)
  garbage <- gc(reset = T)
  cat('Done\n')
  return(ret)
}
