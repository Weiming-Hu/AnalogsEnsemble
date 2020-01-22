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

#' RAnEn::biasCorrection
#' 
#' This is a wrapper function for different bias correction methods.
#' 
#' Currently, it supports the following methods:
#' 
#' \itemize{
#'   \item Insitu bias correction \code{\link{RAnEn::biasCorrectionInsitu}};
#'   \item Neural network bias correction \code{\link{RAnEn::biasCorrectionNeuralNet}};
#' }
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param ... Paramters that are passed to the subsequent function.
#' @param method Which bias correction method to use. It can be one of
#' 'insitu' and 'nnet'.
#' @return An AnEn object.
#' 
#' @examples
#' \dontrun{
#' config <- generateConfiguration('independentSearch')
#' 
#' #
#' # Set up your configuration ...
#' #
#' 
#' AnEn <- generateAnalogs(config)
#' AnEn <- biasCorrection(AnEn, config, forecast.ID = config$observation_id, group.func = mean, na.rm = T,
#'                        keep.bias = T, show.progress = F, overwrite = F, method = 'insitu')
#' AnEn <- biasCorrection(AnEn, config, keep.bias = T, keep.model = T, group.func = mean,
#'                        na.rm = T,  show.progress = F, overwrite = F, parallel = T,
#'                        num.cores = 5, method = 'nnet')
#' }
#' 
biasCorrection <- function(..., method = 'insitu') {
  
  if (method == 'insitu') {
    AnEn <- biasCorrectionInsitu(...)
  } else if (method == 'nnet') {
    AnEn <- biasCorrectionNeuralNet(...)
  } else {
    stop(paste('Unknown correction method', method))
  }
  
  return (AnEn)
}
