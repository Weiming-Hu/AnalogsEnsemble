#  "`-''-/").___..--''"`-._
# (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
# (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#   _ ..`--'_..-_/  /--'_.' ,'
# (il),-''  (li),'  ((!.-'
#
#
# Author: Guido Cervone (cervone@psu.edu), Martina Calovi (mxc895@psu.edu), Laura Clemente-Harding (laura@psu.edu)
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::showVerificationMethods
#' 
#' RAnEn::showVerificationMethods prints all the supported verification methods.
#' 
#' @export
showVerificationMethods <- function() {
  names <- as.character(lsf.str("package:RAnEn", pattern = 'verify*'))
  names <- gsub('^verify', '', names)
  return(names)
}
