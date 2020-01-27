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

AnEnC2R <- function(AnEn) {
  
  if (class(AnEn) != 'AnEn') {
    stop("The conversion only accepts AnEn class.")
  }
  
  config.names <- getConfigNames()
  
  if (config.names$`_ANALOGS_IND` %in% names(AnEn)) {
    AnEn[[config.names$`_ANALOGS_IND`]] <- AnEn[[config.names$`_ANALOGS_IND`]] + 1
  }
  
  if (config.names$`_SIMS_IND` %in% names(AnEn)) {
    AnEn[[config.names$`_SIMS_IND`]] <- AnEn[[config.names$`_SIMS_IND`]] + 1
  }
  
  return(AnEn)
}