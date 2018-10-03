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

#' RAnEn::computeWindSpeed
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#'         Guido Cervone \email{cervone@@psu.edu}
#' 
#' Compute wind speed from U and V component of wind.
#' 
#' @param U A vector of values for U component of wind.
#' 
#' @param V A vector of values for V component of wind.
#'
#' @return A vector of values for wind speed.
#' 
#' @export
computeWindSpeed <- function(U, V) {
  valid <- !is.na(U) & !is.na(V)
  ret <- rep(NA, length(U))
  
  if (length(valid) != 0) {
    ret[valid] = sqrt(U[valid]^2+V[valid]^2)
  }
  
  return(ret)
}