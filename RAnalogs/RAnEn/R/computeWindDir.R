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

#' RAnEn::computeWindDir
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#'         Guido Cervone \email{cervone@@psu.edu}
#' 
#' Compute wind direction from U and V component of wind.
#' 
#' @param U A vector of values for U component of wind.
#' 
#' @param V A vector of values for V component of wind.
#'
#' @return A vector of values for wind speed.
#' 
#' @export
computeWindDir <- function(U, V) {
  
  # Extract valid value indices from U and V.
  valid <- !is.na(U) && !is.na(V)
  
  # Only compute angles when both U and V are valid.
  angles <- rep(NA, length(U))
  if (length(valid) != 0) {
    angle[valid] <- (180/pi * atan2(U[valid],V[valid])) %% 360
    angle[valid][angle[valid]<0]  = angle[valid][angle[valid]<0]+360
  }
  
  return(angle)
}