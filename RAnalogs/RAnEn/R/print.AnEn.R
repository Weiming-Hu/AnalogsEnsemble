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
  cat("Result list with the following members:\n\n")
  empty <- T
  
  existed.names <- sort(names(x))
  
  print.arr.dim <- function(dims, names) {
    stopifnot(length(dims) == length(names))
    lapply(1:length(dims), function(i)
      cat('[', dims[i], ' ', names[i], ']', sep = ''))
  }
  
  # Match names starting with analogs or similarity. These members have 4 dimensions.
  # 
  matched <- grep(pattern = '^(analogs|similarity).*', x = existed.names)
  if (length(matched) != 0) {
    
    for (name in existed.names[matched]) {
      cat("$", name, ":\t", sep = '')
      
      if (length(dim(x[[name]])) != 4) {
        cat('[**Not a 4 dimensional array**]')
      } else {
        print.arr.dim(dim(x[[name]]), c('stations', 'times', 'FLTs', 'members'))
      }
      
      cat("\n")
      empty <- F
    }
    
    existed.names <- existed.names[-matched]
  }
  
  # Match names starting with bias. These members have 3 dimensions.
  # 
  matched <- grep(pattern = '^bias*', x = existed.names)
  if (length(matched) != 0) {
    
    for (name in existed.names[matched]) {
      
      cat("$", name, ":\t", sep = '')
      
      if (length(dim(x[[name]])) != 3) {
        cat('[**Not a 3 dimensional array**]')
      } else {
        print.arr.dim(dim(x[[name]]), c('stations', 'times', 'FLTs'))
      }
      
      cat("\n")
      empty <- F
    }
    
    existed.names <- existed.names[-matched]
  }
  
  # For extra members, I'm going to print either values, dimensions, or lengths
  if (length(existed.names) != 0) {
    empty <- F
    cat("\nExtra members:\n")
    
    for (name in existed.names) {
      cat("$", name, ":\t", sep = '')
      
      if (length(x[[name]]) == 1) {
        # Print the value
        cat('value:', paste(x[[name]]))
        
      } else if (!is.null(dim(x[[name]]))) {
        # Print the dimensions
        cat("dimensions: [", paste0(dim(x[[name]]), collapse = ','), ']', sep = '')
        
      } else {
        # Print the length
        cat("length: [", length(x[[name]]), ']', sep = '')
      }
      
      cat("\n")
    }
    
  }
  
  if (empty) {
    cat('[empty list]\n')
  }
}
