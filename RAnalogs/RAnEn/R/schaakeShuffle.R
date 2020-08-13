# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Guido Cervone (cervone@psu.edu), Martina Calovi (mxc895@psu.edu), Weiming Hu (weiming@psu.edu)
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University

#' RAnEn::schaakeShuffle
#' 
#' RAnEn::schaakeShuffle is a method to improve autocorrelation within ensemble
#' members in the spatial and temporal constructions by reordering the memebers.
#'
#' Although the absolute verification statistics of a particular ensemble (e.g.
#' RMSE, bias, and etc.) are not affected by reordering the members, the reordered
#' ensemble members preserve a better autocorrelation structure when inspected
#' one by one, for example, plotting the first member of all ensembles.
#' 
#' The algorithm is as follows:
#' 
#' ```
#' 
#' -- For each test day
#'       Randomly choose N search days (N is the number of ensemble members)
#'       Establish the linking function B
#'       -- For each FLT
#'          -- For each station
#'             Reorder enmseble members using the linking function B
#'          -- End of stations
#'       -- End of FLTs
#' -- End of test days
#' 
#' ```
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' @author Martina Calovi \email{mxc895@@psu.edu}
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param anen Analogs from \code{\link{generateAnalogs.default}}.
#' The dimensions should be (stations, test times, FLTs, members).
#' @param obs.search The aligned observations with 4 dimensions (parameters, stations,
#' search times, FLTs). However, please make sure there is only 1 parameter. Observations
#' can be created from \code{\link{alignObservations}}.
#' @param allow.na Whether to allow NA values to exist in selected days. Since the the N
#' search days are randomly chosen and there might be NA in the values. Typically, this
#' function will try to find a station index and a lead time index that does not have NA
#' values. But if it fails to do so, you have to consider what you need to do next: 1) 
#' You can check why you have so many NA values in the observations, or 2) you can simply
#' allow sorting with NA, and then all the NA values be placed to the end.
#' @param seed A random seed for reproducibility.
#' 
#' @references 
#' Sperati, Simone, Stefano Alessandrini, and Luca Delle Monache.
#' "Gridded probabilistic weather forecasts with an analog ensemble."
#' Quarterly Journal of the Royal Meteorological Society 143.708 (2017): 2874-2885.
#' 
#' @md
#' @export
schaakeShuffle <- function(anen, obs.search, show.progress = F, allow.na = F, seed = 42) {
  
  set.seed(42)
  
  # Sanity checks
  if (length(dim(obs.search)) != 4) {
    cat("Error: Did you forget to align the search observations (RAnEn::alignObservations)?\n",
        "It should have the dimensions [parameter, stations, search days, FLTs].\n")
    return(NULL)
  }
  
  if (dim(obs.search)[1] != 1) {
    cat("Error: Only 1 parameter is allowed in the search observations.\n")
    return(NULL)
  }
  
  if ( !identical(dim(anen)[c(1, 3)], dim(obs.search)[c(2, 4)]) ) {
    cat("Error: Observations and analogs should have the same number of stations and FLTs.\n")
    return(NULL)
  }
  
  # Get dimensions
  nstations <- dim(anen)[1]
  ntest.days <- dim(anen)[2]
  nflts <- dim(anen)[3]
  nmembers <- dim(anen)[4]
  
  nsearch.days <- dim(obs.search)[3]
  
  anen.ss <- array(NA, dim=c(nstations, ntest.days, nflts, nmembers))
  
  if (show.progress) {
    pb <- txtProgressBar(max = ntest.days * nflts * nstations, style = 3)
    counter <- 0
  }
  
  for (i.test.day in 1:ntest.days) {
    
    # Randomly choose N search days
    selected <- sample(1:nsearch.days, size = nmembers)
    
    # Fnd a combination of station and flt index from the observations
    # that does not contain any NA values.
    #
    exit_loops <- FALSE

    for (i.selected.station in sample.int(nstations, nstations)) {
      
      if (exit_loops) {
        break
      }

      for (i.selected.flt in sample.int(nflts, nflts)) {
        selected.values <- obs.search[1, i.selected.station, selected, i.selected.flt] 

        if (allow.na | !any(is.na(selected.values))) {
          exit_loops <- TRUE
          break
        }
      }
    }
    
    if (!allow.na & any(is.na(selected.values))) {
      msg <- paste(selected, collapse = ',')
      msg <- paste0("\n\nI randomly selected several days (", msg,
                    ") but I can't find a station index and a",
                    "lead time index that contains no NA values!\n\n",
                    "Maybe you want to use allow.na = T to sort",
                    " all NA values to the end. Read more in `?schaakeShuffle`.")
      stop(msg)
    }
    
    # Establish the linking function
    selected.order <- order(selected.values)
    
    for (i.flt in 1:nflts) {
      for (i.station in 1:nstations) {
        
        # Reorder enmseble members using the linking function
        sorted.members <- sort( anen[i.station, i.test.day, i.flt, ] )
        anen.ss[i.station, i.test.day, i.flt, ] <- sorted.members[selected.order]
        
        if (show.progress) {
          counter <- counter + 1
          setTxtProgressBar(pb, counter)
        }
        
      }
    }
  }
  
  if (show.progress) {
    close(pb)
  }
  
  return(anen.ss)
}

