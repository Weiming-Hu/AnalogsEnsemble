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

#' RAnEn::readAnEn
#' 
#' RAnEn::readAnEn can read AnEn results from NetCDF files that are generated 
#' from analogGenerator. It supports reading analogs and similarity results.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param file.analogs The path to analog NetCDF file.
#' @param file.similarity The path to similarity NetCDF file.
#' @param verbose Whether to print progress.
#' 
#' @return An AnEn class object.
#' @export
readAnEn <- function(file.analogs, file.similarity = NA, verbose = F) {
  require(ncdf4)
  AnEn <- list()
  class(AnEn) <- 'AnEn'
  
  if (file.exists(file.analogs)) {
    if (verbose) cat('Reading analogs from', file.analogs, '\n')
    
    nc <- nc_open(file.analogs)
    AnEn$analogs <- ncvar_get(nc, 'Analogs')
    
    if (verbose) cat('Converting analog format ...\n')
    AnEn <- AnEnC2R(AnEn, 'analogs')
    nc_close(nc)
    
  } else {
    stop(paste(file.analogs, 'does not exists.'))
  }
  
  if (!is.na(file.similarity)) {
    if (file.exists(file.similarity)) {
      if (verbose) cat('Reading similarity from', file.similarity, '...\n')
      
      nc <- nc_open(file.similarity)
      AnEn$similarity <- ncvar_get(nc, 'SimilarityMatrices')
      
      if (verbose) cat('Converting similairty format ...\n')
      AnEn <- AnEnC2R(AnEn, 'similarity')
      nc_close(nc)
      
    } else {
      stop(paste(file.similarity, 'does not exists.'))
    }
  }
  
  if (verbose) cat('Done!\n')
  
  return (AnEn)
}
