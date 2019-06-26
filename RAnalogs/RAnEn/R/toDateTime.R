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

#' RAnEn::toDate
#' 
#' RAnEn::toDate is a sugar function to convert seconds to date time objects.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' 
#' @md
#' @export
toDate <- function(time.in.secs, origin = "1970-01-01", tz = "UTC") {
  return(as.POSIXct(time.in.secs, origin = origin, tz = tz) )
}