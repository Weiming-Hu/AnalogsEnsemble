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

#' RAnEn::generateSearchStations
#' 
#' RAnEn::generateSearchStations is the search station computation routine included
#' in the function \code{\link{generateAnalogs}}. This function computes the
#' search stations of each test station. Search stations are
#' represented using their index. Each column includes the search station indices for
#' each test station. The available constraints for calculating the search stations
#' include 1) Euclidean distances; 2) number of nearest neighbors; 3) station classifiers.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param test_stations_x Xs for test stations.
#' @param test_stations_y Ys for test stations.
#' @param search_stations_x Xs for search stations.
#' @param search_stations_y Ys for search stations.
#' @param max_num_search_stations The maximum number of search stations that will be stored.
#' If you are using the parameter `num_nearest`, it is a good practice to set this to be the 
#' same as `num_nearest`; if you are using `distance` only, please set this to be the maximum
#' number of stations that can be found as an upper bound to limit memory consumption. `NA`
#' values might be present if this is set too big. This only affect the memory usage.
#' @param distance The buffer distance from each test station to look for search stations.
#' @param num_nearest The number of nearest neighbor to select.
#' @param test_stations_classifier A vector for test station classifiers.
#' @param search_stations_classifier A vector for search station classifiers.
#' @param verbose Verbose level.
#' 
#' @return A matrix for search stations. Each column includes the search station indices
#' for each test station.
#' 
#' @import Rcpp BH
#' 
#' @useDynLib RAnEn
#' 
#' @md
#' @export
generateSearchStations <- function(
  test_stations_x, test_stations_y,
  search_stations_x, search_stations_y,
  max_num_search_stations = 0,
  distance = 0,
  num_nearest = 0,
  test_stations_classifier = vector('numeric', 0), 
  search_stations_classifier = vector('numeric', 0), 
  verbose = 3) {
  
  # Sanity checks
  if (!is.vector(test_stations_x)) test_stations_x <- as.vector(test_stations_x)
  if (!is.vector(test_stations_y)) test_stations_y <- as.vector(test_stations_y)
  if (!is.vector(search_stations_x)) search_stations_x <- as.vector(search_stations_x)
  if (!is.vector(search_stations_y)) search_stations_y <- as.vector(search_stations_y)
  
  stopifnot(is.numeric(test_stations_x))
  stopifnot(is.numeric(test_stations_y))
  stopifnot(is.numeric(search_stations_x))
  stopifnot(is.numeric(search_stations_y))
  
  stopifnot(length(test_stations_x) == length(test_stations_y))
  stopifnot(length(search_stations_x) == length(search_stations_y))
  
  if (identical(num_nearest, NA) && identical(distance, NA)) {
    stop('Must provide at least on of num_nearest or distance.')
  } else if (!identical(num_nearest, NA)) {
    if (num_nearest != 0) {
      stopifnot(num_nearest <= length(search_stations_x)) 
    }
  }
  
  mat <- .generateSearchStations(
    test_stations_x, test_stations_y,
    search_stations_x, search_stations_y,
    max_num_search_stations,
    distance, num_nearest,
    test_stations_classifier,
    search_stations_classifier, verbose
  )
  
  mat <- mat + 1
  
  if (verbose >= 3)  cat("Done (generateSearchStations)!\n")
  
  return(mat)
}
