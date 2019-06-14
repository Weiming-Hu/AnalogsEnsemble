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

#' RAnEn::AnEnC2R
#' 
#' RAnEn::AnEnC2R converts the C indices to R indices. This function is not
#' for the general use.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' @param AnEn An AnEn class object.
#' @param member The name of member to convert indices.
#' @return An AnEn class object.
#' @export
AnEnC2R <- function(AnEn, member) {
  if (class(AnEn) != 'AnEn') {
    stop("The conversion only accepts AnEn class.")
  }
  
  if (member == 'similarity') {
    stopifnot('similarity' %in% names(AnEn))
    
    AnEn$similarity <- aperm(AnEn$similarity, length(dim(AnEn$similarity)):1)
    
    ori <- as.vector(AnEn$similarity[, , , , 3])
    ori[which(ori >= 0)] <- ori[which(ori >= 0)] + 1
    AnEn$similarity[, , , , 3] <- ori
    
    AnEn$similarity[, , , , 2] <- AnEn$similarity[, , , , 2, drop = F] + 1
    
  } else if (member == 'analogs') {
    stopifnot('analogs' %in% names(AnEn))
    
    # Convert station index from C counting to R counting
    AnEn$analogs[, , , , 2] <- AnEn$analogs[, , , , 2, drop = F] + 1
    
    # Convert time index from C to R if the index is non-negative
    ori <- as.vector(AnEn$analogs[, , , , 3])
    ori[which(ori >= 0)] <- ori[which(ori >= 0)] + 1
    AnEn$analogs[, , , , 3] <- ori
  }
  
  return(AnEn)
}