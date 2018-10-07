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

#' RAnEn::checkOpenMP
#' 
#' This function checks whether OpenMP is supported in RAnEn library.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param silent Whether to suppress output.
#' 
#' @return A boolean whether OpenMP is supported.
#' 
#' @import Rcpp BH
#' 
#' @importFrom Rcpp evalCpp
#' 
#' @useDynLib RAnEn
#' 
#' @export
checkOpenMP <- function(silent = F) {
  exist <- .checkOpenMP()
  
  if (!silent) {
    if (exist) cat(paste("OpenMP is supported.\n"))
    else cat(paste("OpenMP is not supported.\n"))
  }
  
  return (exist)
}