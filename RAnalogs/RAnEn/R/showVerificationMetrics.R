#  "`-''-/").___..--''"`-._
# (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
# (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#   _ ..`--'_..-_/  /--'_.' ,'
# (il),-''  (li),'  ((!.-'
#
#
# Author: Weiming Hu (weiming@psu.edu)
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::showVerificationMetrics
#' 
#' RAnEn::showVerificationMetrics prints all the supported verification metrics.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @export
showVerificationMetrics <- function() {
  names <- as.character(lsf.str("package:RAnEn", pattern = 'verify*'))
  names <- gsub('^verify', '', names)
  return(names)
}
