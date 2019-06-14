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

#' RAnEn::readConfig
#' 
#' RAnEn::readConfig reads a specific parameter from a configuration file (.cfg). The
#' configuration file is usually used by the C++ utility.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' @param config.file The full path to the configuration file (.cfg)
#' @param parameter The name of the configuration parameter to read.
#' @param return.numeric If this is TRUE, the returned vector will be converted to 
#' a numeric vector. If this is FALSE, the returned vector will be a character vector.
#' @return A vector for the values of the specified configuration parameter.
#' @export
readConfig <- function(config.file, parameter, return.numeric = F) {
  stopifnot(file.exists(config.file))
  
  # Read file content as lines
  con <- file(config.file, 'r')
  lines <- readLines(con = con)
  close(con)
  
  # Remove comment lines and empty lines
  to.be.removed <- grep('(^#.*)|(^$)', lines)
  if (length(to.be.removed) != 0) {
    lines <- lines[-to.be.removed]
  }
  
  # Find lines with the specified parameter
  to.be.kept <- grep(paste('^', parameter, '.*$', sep = ''), lines)
  
  # Parse the matched lines
  lines <- lines[to.be.kept]
  lines <- gsub(paste('(', parameter, ')|(=)|( )', sep = ''), '', lines)
  
  if (return.numeric) {
    return(as.numeric(lines))
  } else {
    return(lines)
  }
}