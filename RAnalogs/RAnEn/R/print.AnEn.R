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

#' RAnEn::print.AnEn
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' Overload print function for class AnEn
#' 
#' @param x an AnEn object
#' 
#' @examples 
#' AnEn <- list()
#' class(AnEn) <- 'AnEn'
#' print(AnEn)
#' 
#' @export
print.AnEn <- function (x) {
  cat("Class: AnEn list\n")
  empty <- T
  
  if ('analogs' %in% names(x)) {
    empty <- F
    cat("Member 'analogs': [test station][test time][FLT][member][type]\n")
    cat(dim(x$analogs))
    cat(" (value, search station, search observation time)")
    cat("\n")
  }
  
  if ('similarity' %in% names(x)) {
  	empty <- F
  	cat("Member 'similarity': [test station][test time][FLT][member][type]\n")
  	cat(dim(x$similarity))
  	cat(" (value, search station, search forecast time)")
  	cat("\n")
  }
  
  if ('mapping' %in% names(x)) {
    empty <- F
    cat("Member 'mapping': [FLT][forecast time] ")
    cat(dim(x$mapping))
    cat("\n")
  }
  
  if ('searchStations' %in% names(x)) {
  	empty <- F
  	cat("Member 'searchStations': [search station][test station] ")
  	cat(dim(x$searchStations))
  	cat("\n")
  }
  
  if (empty) {
    cat('[empty list]\n')
  }
}
