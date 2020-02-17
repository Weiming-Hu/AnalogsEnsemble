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

#' RAnEn::print.Forecasts
#' 
#' RAnEn::print.Forecasts print a Forecasts object.
#' @param x A Forecasts object
#' @md
#' @export
print.Forecasts <- function(x) {
  printTemplates(x)
}


#' RAnEn::print.Observations
#' 
#' RAnEn::print.Observations print a Forecasts object.
#' @param x A Forecasts object
#' @md
#' @export
print.Observations <- function(x) {
  printTemplates(x)
}

# Internal function for printing
printTemplates <- function(x) {
  
  # Determine the type
  type <- ifelse(
    class(x) == 'Forecasts',
    yes = 'Forecasts',
    no = 'Observations')
  
  # Get expected names
  expected.names <- `if`(
    type == 'Forecasts',
    names(generateForecastsTemplate()),
    names(generateObservationsTemplate()))
  
  # Define data dimensions
  data.dim.names <- `if`(
    type == 'Forecasts',
    c('parameter', 'stations', 'times', 'flts'),
    c('parameter', 'stations', 'times'))
  
  # Get a config
  config <- new(Config)
  
  # Get the name pairs
  pairs <- config$getNames()
  
  cat(type, ":\n")
  
  for (name in c(expected.names)) {
    
    member <- x[[name]]
    
    cat("$", name, ": ", sep = '')
    
    if (is.null(member)) {
      cat('** empty **')
    } else {
      
      if (name == pairs$`_DATA`) {
        # Data is an array
        if (length(data.dim.names) != length(dim(member))) {
          warning("The member Data has incorrect dimensions")
        }
        
        cat('[', paste(dim(member), data.dim.names, collapse = ', '), ']', sep = '')
        
      } else {
        # Other expected members should just be a vector
        cat('[', length(x[[name]]), '] ', sep = '')
        cat(head(x[[name]]))
      } 
    }
    
    cat('\n')
  }
  
  # Extract the extra members if this object has any
  all.names <- names(x)
  extra.names <- setdiff(all.names, expected.names)
  
  if (length(extra.names) != 0) {
    cat("\nExtra members found:", paste(extra.names, collapse = ', '), '\n')
  }
}
