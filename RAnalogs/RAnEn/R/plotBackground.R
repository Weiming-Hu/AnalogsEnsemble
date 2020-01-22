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

#' RAnEn::plotBackground
#' 
#' RAnEn::plotBackground is a sugar function to plot a background color.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' 
#' @export
plotBackground <- function(col="lightgrey") {
  rect(par("usr")[1],par("usr")[3],par("usr")[2],par("usr")[4],col = "lightgray")
}