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

#' RAnEn::print.list
#' 
#' RAnEn::print.list prints a list in pretty format
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param x The object to print
#' @param names_to_print The names to print. Default to all names.
#' 
#' @md
#' @export
print.list <- function(x) {
  printExtra(x, names(x))
}

printExtra <- function(x, names_to_print) {
  
  for (name in names_to_print) {
    
    if (!(name %in% names(x))) {
      stop(paste(name, "does not exists (Internal from RAnEn::print.list)"))
    }
    
    cat("$", name, ":\t", sep = '')
    
    if (length(x[[name]]) == 1 & !is.list(x[[name]])) {
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
