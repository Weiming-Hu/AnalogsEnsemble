# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <wuh20@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University

#' RAnEn::generateCircleCoords
#' 
#' @author Weiming Hu, \email{weiming@@psu.edu}
#' 
#' Generate a number of sequential point coordinates that
#' lie on a circle based on the center coordinates of the
#' circle and the radius.
#' 
#' @param x x coordinate of the center.
#' @param y y coordinate of the center.
#' @param radius radius of the circle in user's unit.
#' @param np number of points to generate on the circle.
#' By default it is set to 100.
#' 
#' @return a matrix with a dimension of np x 2.
#' 
#' @examples
#' pts <- generateCircleCoords(3, 5, 2)
#' plot(pts, pch = 19, cex = 1)
#' points(3, 5, pch = 19, col = 'red')
#' 
#' @export
generateCircleCoords <- function(x, y, radius, np = 100) {
  theta <- seq(0, pi/2, length.out = np)
  xs <- sin(theta) * radius
  ys <- cos(theta) * radius
  
  xs <- c(xs, xs[length(xs):1], -xs, -xs[length(xs):1])
  ys <- c(ys, -ys[length(ys):1], -ys, ys[length(ys):1])
  
  coords <- cbind(xs + x, ys + y)
  colnames(coords) <- c('x', 'y')
  
  return(coords)
}
