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


#' RAnEn::val2Col
#' 
#' RAnEn::val2Col maps each value to a color in the input color sequence.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' @author Martina Calovi \email{mxc895@@psu.edu}
#' @author Laura Clemente-Harding \email{laura@@psu.edu}
#' 
#' @param x A vector of values.
#' @param min The minimum value.
#' @param max The maximum value.
#' @param na.rm Whether to remove NA values.
#' @param col A vector of colors to choose from.
#' 
#' @export
val2Col <- function( x, min = min(x, na.rm), max = max(x, na.rm), na.rm = F, col) {
  x <- round( ( (x-min) / (max-min) ) * (length(col)-1)  + 1 )
  x[x<1] = 1
  x[x>length(col)] = length(col)
  return(col[x])
}

