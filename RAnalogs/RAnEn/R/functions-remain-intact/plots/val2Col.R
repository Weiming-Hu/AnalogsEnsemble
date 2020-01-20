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
#' RAnEn::val2Col maps each value to a color in the input colors.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' @author Martina Calovi \email{mxc895@@psu.edu}
#' @author Laura Clemente-Harding \email{laura@@psu.edu}
#' 
#' @param x A vector of values.
#' @param col A vector of colors to choose from.
#' @param x.min The minimum value of the interval to start mapping colors.
#' Smaller values will be mapped to the first color. Please see the example.
#' @param x.max The maximum value of the interval to start mapping colors.
#' Larger values will be mapped to the last color. Please see the example.
#' @param na.rm Whether to remove NA values.
#' 
#' @examples 
#' 
#' # Suppose we have a vector of numeric numbers
#' x <- runif(1000)
#' 
#' # Suppose we have some colors to which we would like to map the numbers
#' cols.all <- c('green', 'red', 'yellow', 'grey', 'blue')
#' 
#' # Define the limit for values to be mapped to colors
#' x.min <- 0.4
#' x.max <- 0.8
#' 
#' # Generate colors for each number
#' cols.gen <- val2Col(x, cols.all, x.min = x.min, x.max = x.max)
#' 
#' # See the output
#' plot(x, pch = 16, cex = 1.5, col = cols.gen)
#' abline(h = c(x.min, x.max))
#' 
#' @export
val2Col <- function( x, col, x.min = NULL, x.max = NULL, na.rm = F) {
  
  if (length(col) == 1) stop('At least 2 colors are needed.')
  
  # Translate the minimum and the maximum values
  x.min <- ifelse(test = is.null(x.min), yes = min(x, na.rm), no = x.min)
  x.max <- ifelse(test = is.null(x.max), yes = max(x, na.rm), no = x.max)
  
  # Cluster the numbers
  x.cut <- cut(x, breaks = seq(from = x.min, to = x.max, length.out = length(col) + 1))
  
  # Deal with min/max values if any
  clusters <- levels(x.cut)
  x.cut[which(x <= x.min)] <- clusters[1]
  x.cut[which(x >= x.max)] <- clusters[length(clusters)]
  
  return(col[x.cut])
}
