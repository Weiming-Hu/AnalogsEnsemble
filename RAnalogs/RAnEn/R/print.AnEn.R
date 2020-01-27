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
  cat("Class: AnEn list\n\n")
  empty <- T
  
  existed.names <- sort(names(x))
  
  # Match names starting with analogs. These members are assumed to
  # be analogs with 5 dimensions or 4 dimensions.
  # 
  matched <- grep(pattern = '^analogs.*', x = existed.names)
  if (length(matched) != 0) {
    
    for (name in existed.names[matched]) {
      
      if (length(dim(x[[name]])) == 5) {
        cat(paste("Member '", name, "': [station][test time][FLT][member][value, station, observation time]", sep = ''), '\n')
      } else if (length(dim(x[[name]])) == 4) {
        cat(paste("Member '", name, "': [station][test time][FLT][member]", sep = ''), '\n')
      }
      
      cat(dim(x[[name]]))
      cat("\n")
      empty <- F
    }
    
    existed.names <- existed.names[-matched]
    cat("\n")
  }
  
  # Match names starting with similarity. These members are assumed to
  # be similarity matrices with 5 dimensions.
  # 
  matched <- grep(pattern = '^similarity.*', x = existed.names)
  if (length(matched) != 0) {
    
    for (name in existed.names[matched]) {
      empty <- F
      cat(paste("Member '", name, "': [station][test time][FLT][member][value, station, forecast time]", sep = ''), '\n')
      cat(dim(x[[name]]))
      cat("\n")
      empty <- F
    }
    
    existed.names <- existed.names[-matched]
    cat("\n")
  }
  
  # Match names starting with bias. These members are assumed to
  # be bias matrices with 3 dimensions.
  # 
  matched <- grep(pattern = '^bias*', x = existed.names)
  if (length(matched) != 0) {
    
    for (name in existed.names[matched]) {
      cat(paste("Member '", name, "': [test station][test time][FLT]", sep = ''), '\n')
      cat(dim(x[[name]]))
      cat("\n")
      empty <- F
    }
    
    existed.names <- existed.names[-matched]
    cat("\n")
  }
  
  if (empty) {
    cat('[empty list]\n')
  }
  
  if (length(existed.names) != 0) {
    cat("Extra members:\n")
    cat(paste(existed.names, collapse = ', '), '\n')
  }
}
