#  "`-''-/").___..--''"`-._
# (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
# (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#   _ ..`--'_..-_/  /--'_.' ,'
# (il),-''  (li),'  ((!.-'
#
#
# Author: Weiming Hu <weiming@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::check.package
#' 
#' RAnEn::check.package is the internal functions checking whether a
#' package is installed or not.
#' 
#' @md
#' 
#' @keywords internal
check.package <- function(name) {
  if (!requireNamespace(name, quietly = T)) {
    msg <- paste('Package', name, 'is needed but not installed.',
                 'Please install', name)
    stop(msg)
  }
  
  return(NULL)
}
