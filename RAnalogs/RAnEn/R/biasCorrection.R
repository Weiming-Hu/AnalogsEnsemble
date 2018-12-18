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
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' This is a wrapper function for different bias correction methods.
#' 
#' Currently, it supports the following methods:
#' - Insitu bias correction \code{\link{RAnEn::biasCorrectionInsitu}};
#' - Neural network bias correction \code{\link{RAnEn::biasCorrectionNeuralNet}};
#' 
#' @param ... Paramters that are passed to the subsequent function.
#' @param method Which bias correction method to use. It can be one of
#' 'insitu' and 'nnet'.
#' 
#' @return An AnEn object.
#' 
#' @export
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