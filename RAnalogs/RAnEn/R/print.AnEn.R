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
#' Overload print function for class AnEn
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
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
  
  existed.names <- names(x)
  
  if ('analogs' %in% names(x)) {
    empty <- F
    cat("Member 'analogs': [test station][test time][FLT][member][type]\n")
    cat(dim(x$analogs))
    cat(" (value, search station, search observation time)")
    cat("\n")
    existed.names <- existed.names[-which(existed.names == 'analogs')]
  }
  
  if ('similarity' %in% names(x)) {
  	empty <- F
  	cat("Member 'similarity': [test station][test time][FLT][member][type]\n")
  	cat(dim(x$similarity))
  	cat(" (value, search station, search forecast time)")
  	cat("\n")
  	existed.names <- existed.names[-which(existed.names == 'similarity')]
  }
  
  if ('mapping' %in% names(x)) {
    empty <- F
    cat("Member 'mapping': [FLT][forecast time] ")
    cat(dim(x$mapping))
    cat("\n")
    existed.names <- existed.names[-which(existed.names == 'mapping')]
  }
  
  if ('searchStations' %in% names(x)) {
    empty <- F
    cat("Member 'searchStations': [search station][test station] ")
    cat(dim(x$searchStations))
    cat("\n")
    existed.names <- existed.names[-which(existed.names == 'searchStations')]
  }
  
  if ('std' %in% names(x)) {
  	empty <- F
  	cat("Member 'std': [parameter][test station][FLT] ")
  	cat(dim(x$std))
  	cat("\n")
  	existed.names <- existed.names[-which(existed.names == 'std')]
  }
  
  if ('analogs.cor.insitu' %in% names(x)) {
    empty <- F
    cat("Member 'analogs.cor.insitu': [test station][test time][FLT][member][type]\n")
    cat(dim(x$analogs.cor.insitu))
    cat(" (value, search station, search observation time)")
    cat("\n")
    existed.names <- existed.names[-which(existed.names == 'analogs.cor.insitu')]
  }
  
  if ('bias.insitu' %in% names(x)) {
    empty <- F
    cat("Member 'bias.insitu': [test station][test time][FLT] ")
    cat(dim(x$bias.insitu))
    cat("\n")
    existed.names <- existed.names[-which(existed.names == 'bias.insitu')]
  }
  
  if ('analogs.cor.nnet' %in% names(x)) {
    empty <- F
    cat("Member 'analogs.cor.nnet': [test station][test time][FLT][member][type]\n")
    cat(dim(x$analogs.cor.nnet))
    cat(" (value, search station, search observation time)")
    cat("\n")
    existed.names <- existed.names[-which(existed.names == 'analogs.cor.nnet')]
  }
  
  if ('bias.nnet' %in% names(x)) {
    empty <- F
    cat("Member 'bias.nnet': [test station][test time][FLT] ")
    cat(dim(x$bias.nnet))
    cat("\n")
    existed.names <- existed.names[-which(existed.names == 'bias.nnet')]
  }
  
  if ('bias.model.nnet' %in% names(x)) {
    empty <- F
    cat("Member 'bias.model.nnet':  ")
    cat(class(x$bias.model.nnet))
    cat("\n")
    existed.names <- existed.names[-which(existed.names == 'bias.model.nnet')]
  }
  
  if (empty) {
    cat('[empty list]\n')
  }
  
  if (length(existed.names) != 0) {
    cat("Some member names are not recognized:",
        paste(existed.names, collapse = ', '), '\n')
  }
}
