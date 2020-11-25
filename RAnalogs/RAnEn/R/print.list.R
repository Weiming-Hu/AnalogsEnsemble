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
#' @param names_to_print The names to print. Default to all names. Only
#' support including names in the root group.
#' @param recursive Whether to print all variables from nest-groups
#' 
#' @md
#' @export
print.list <- function(x, recursive = F) {
  printExtra(x, names(x), recursive = recursive)
}

printExtra <- function(x, names_to_print, recursive = F, indent = '') {
  
  for (name in names_to_print) {
    
    if (!(name %in% names(x))) {
      stop(paste(name, "does not exists (Internal from RAnEn::printExtra)"))
    }
    
    cat(indent, "$", name, ":\t", sep = '')
    
    if (length(x[[name]]) == 1 & !is.list(x[[name]])) {
      # Print the value
      cat('value:', paste(x[[name]]))
    } else if (!is.null(dim(x[[name]]))) {
      # Print the dimensions
      cat("dimensions: [", paste0(dim(x[[name]]), collapse = ','), ']', sep = '')
    } else if (is.list(x[[name]])) {
      cat("list: [", length(x[[name]]), ']', sep = '')
    } else {
      # Print the length
      cat("length: [", length(x[[name]]), ']', sep = '')
    }
    
    cat("\n")
    
    if (recursive & is.list(x[[name]])) {
      printExtra(x[[name]], names(x[[name]]), recursive, paste0('   ', indent))
    }
  }
}
