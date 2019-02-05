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

#' RAnEn::version
#' 
#' Print the version of RAnEn.
#'
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param newline A boolean for whether to add a new line at the end of the printing message.
#'
#' @export
version <- function(newline = T) {
  version <- read.dcf(file.path(lib, pkg, "DESCRIPTION"), "Version")
  if (newline)
    cat(version, '\n')
  else
    cat(version)
}
