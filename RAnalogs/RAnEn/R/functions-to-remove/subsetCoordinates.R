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

#' RAnEn::subsetCoordinates
#' 
#' RAnEn::subsetCoordinates extracts the coordinates within an predefined extent
#' or the coordinates that are closest to a set of target coordinates based
#' on Eucleadian distances. This function can also write the corresponding
#' IDs to the extracted coordinates into a file so that it can be used
#' in [other executables provided](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/apps)
#' in the PAnEn packages.
#'
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param xs A vector with X coordinates.
#' @param ys A vector with Y coordinates.
#' @param poi Points of interest, or target points. It can be either a named
#' vector with `left`, `right`, `top`, and `bottom` or a data frame for
#' a set of target point coordinates as rows with variable names `X` and `Y`.
#' @param file.output A file name to write the ID of subset stations.
#' @param arg.name If you are going to use the output file with `fileSlice`,
#' this is `slice-stations`; if the app is `gribConverter`, this is `subset-stations`.
#' 
#' @return A data frame with ID and coordinates for subset stations. The column `ID.C` 
#' should be used for C++ programs. The column `ID.R` should be used in R. 
#' 
#' @examples 
#' 
#' ####################################################################
#' #                       Use Case 1                                 #
#' ####################################################################
#' 
#' # Define the forecast grid coordinates
#' forecast.grids <- expand.grid(X = 1:10, Y = 1:10)
#' 
#' # Define the observation coordinates
#' obs.coords <- data.frame(X = c(3.2, 7.9), Y = c(5.1, 4.2))
#' 
#' # Find the closest forecast grid to each observation location
#' subset.coords <- subsetCoordinates(
#'   forecast.grids$X, forecast.grids$Y, obs.coords)
#' 
#' # Visualization
#' plot(forecast.grids$X, forecast.grids$Y, pch = 16,
#'      cex = 1, col = 'grey', asp = 1, xlab = '', ylab = '')
#' points(obs.coords$X, obs.coords$Y, pch = 16, cex = 1, col = 'red')
#' points(subset.coords$X, subset.coords$Y, cex = 1.5, lwd = 1.5)
#' legend('top', legend = c('Forecast Grid', 'Observation', 'Cloest Grid'),
#'        col = c('grey', 'red', 'black'), pch = c(16, 16, 1),
#'        horiz = T, cex = 1.2)
#' 
#' ####################################################################
#' #                       Use Case 2                                 #
#' ####################################################################
#' 
#' # Define a region of interest
#' extent <- c('left' = 2.5, 'right' = 8,
#'             'top' = 8.9, 'bottom' = 2)
#' 
#' # Subset the forecast grids within the extent
#' subset.coords <- subsetCoordinates(
#'   forecast.grids$X, forecast.grids$Y, extent)
#' 
#' # Visualization
#' plot(forecast.grids$X, forecast.grids$Y, pch = 16,
#'      cex = 1, col = 'grey', asp = 1, xlab = '', ylab = '')
#' rect(xleft = extent['left'], xright = extent['right'],
#'      ybottom = extent['bottom'], ytop = extent['top'],
#'      cex = 1, col = NA, lwd = 1, lty = 'dashed')
#' points(subset.coords$X, subset.coords$Y, cex = 1.5, lwd = 1.5)
#' legend('top', legend = c('Forecast Grid', 'Subset Grid'),
#'        col = c('grey', 'black'), pch = c(16, 1),
#'        horiz = T, cex = 1.2)
#' 
#' @useDynLib RAnEn
#' 
#' @md
#' @export
subsetCoordinates <- function(
  xs, ys, poi, file.output = NULL,
  arg.name = stop('Specify the argument name!')) {
  
  # Sanity check
  stopifnot(length(xs) == length(ys))
  
  # Determine the type of the inpur poi
  if (is.data.frame(poi)) {
    if (all(c('X', 'Y') %in% names(poi))) {
      poi.type <- 'data frame'
    } else {
      stop('The data frame poi should have variable names x and y.')
    }
    
  } else if (is.vector(poi)) {
    if (!all(c('left', 'right', 'bottom', 'top') %in% names(poi))) {
      stop('The vector poi should be a named vector with left, right, bottom, and top.')
    }
    
    poi.type <- 'vector'
  } else {
    stop('The input poi can either be a named vector or a data frame.')
  }
  
  # Create a data frame with the entire set of coordinates
  # from xs and ys. The ID column is the index counting 
  # from 0 because of the convention in NetCDF files.
  # 
  df <- data.frame(
    ID.C = (1:length(xs)) - 1,
    ID.R = 1:length(xs),
    X = xs, Y = ys)
  
  if (poi.type == 'data frame') {
    
    ret <- generateSearchStations(
      test_stations_x = poi$X, test_stations_y = poi$Y,
      search_stations_x = df$X, search_stations_y = df$Y,
      num_nearest = 1, verbose = 1)
    
    df <- df[ret, ]
    
  } else if (poi.type == 'vector') {
    xmin <- poi['left']
    xmax <- poi['right']
    ymin <- poi['bottom']
    ymax <- poi['top']
    
    # Extract points from the extent
    df <- subset(df,  X >= xmin & X <= xmax &
                   Y >= ymin & Y <= ymax)
    
  } else {
    stop('Unknown poi.type.')
  }
  
  if (!identical(file.output, NULL)) {
    # Disable scientific notation
    old.value <- getOption("scipen")
    options(scipen = 999)
    
    if (file.exists(file.output)) {
      stop('File exists. Skip writing.')
    }
    
    file.lines <- c(
      paste("# Author:", paste("RAnEn", packageVersion("RAnEn"))), 
      paste("# Time of creation:", format(
        Sys.time(), format = "%Y/%m/%d %H:%M:%S UTC%z")),
      '\n', paste(arg.name, '=', df$ID.C))
    
    con <- file(file.output, "w")
    writeLines(file.lines, con = con)
    close(con)
    
    cat('Arguments written to file', file.output, '\n')
    options(scipen = old.value)
  }
  
  return(df)
}
