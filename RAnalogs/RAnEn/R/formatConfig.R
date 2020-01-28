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

#' RAnEn::formatConfig
#' 
#' RAnEn::formatConfig
#' 
#' @md
#' @export
formatConfig <- function(config) {
  
  if (class(config) != "Configuration") {
    stop("Input should be a Configuration list")
  }
  
  # Extract names
  names <- names(config)
  expected.names <- getConfigNames()
  
  # Define a name dictionary
  dict <- list(
    debug = "_REMOVE_",
    advanced = "_REMOVE_",
    preserve_std = "_REMOVE_",
    time_match_mode = "_REMOVE_",
    extend_observations = "_REMOVE_",
    preserve_search_stations = "_REMOVE_",
    
    test_forecasts = "_DEPRECATED_",
    search_forecasts = "_DEPRECATED_",
    
    test_times_compare = expected.names$`_TEST_TIMES`,
    search_observations = expected.names$`_OBS`,
    search_times_compare = expected.names$`_SEARCH_TIMES`
  )
  
  # Compare and find any unexpected names
  unexpected.names <- setdiff(names, expected.names)
  
  if (length(unexpected.names) == 0) {
    return(config)
  }
  
  ##################################################
  #            Resolve naming issues               #
  ##################################################
  unrecognized.names <- c()
  deprecated.names <- c()
  
  msg <- paste0("********************************************\n",
                "You are using old names in configuration.\n",
                "I know how to change the following names:\n", sep = '')
  
  for (unexpected.name in unexpected.names) {
    
    if (unexpected.name %in% names(dict)) {
      # If the unexpected name is found in the dictionary,
      # I know how to deal with this case.
      # 
      
      if (dict[[unexpected.name]] == '_REMOVE_') {
        # Remove this name
        config[[unexpected.name]] <- NULL
        msg <- paste0(msg, '- ', unexpected.name, ' has been removed.\n')
      } else if (dict[[unexpected.name]] == '_DEPRECATED_') {
        # This name is forbidden. An error message will be generated.
        deprecated.names <- c(deprecated.names, unexpected.name)
      } else {
        # Change the name to an expected name
        config[[dict[[unexpected.name]]]] <- config[[unexpected.name]]
        config[[unexpected.name]] <- NULL
        msg <- paste0(msg, '- ', unexpected.name, ' has been changed to ', dict[[unexpected.name]], '\n')
      }
      
    } else {
      # If the unexpected name is not found, there is a problem.
      unrecognized.names <- c(unrecognized.names, unexpected.name)
    }
  }
  
  if (length(deprecated.names) != 0) {
    msg <- paste0(msg, "\nThese names are deprecated: ", paste(deprecated.names, collapse = ','))
    stop(msg)
  }
  
  if (length(unrecognized.names) != 0) {
    msg <- paste0(msg, "\nThese names are not recognized: ", paste(unrecognized.names, collapse = ','))
    stop(msg)
  }
  
  msg <- paste0(msg, 'All names resolved!\n', '********************************************\n')
  
  if (config[[expected.names$`_VERBOSE`]] > 0) {
    cat(msg)
  }
  
  return(config)
}
