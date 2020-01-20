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
#' @param split The number of times to split the longest dimension when reading.
#' For example, during the reading of analogs with 5 dimensions, if `split` is 
#' 3, the longest dimension of analogs will be divided 3 times into 4 chunks.
#' This is useful when your file cannot be read at once.
#' 
#' @return An AnEn class object.
#' 
#' @md
#' @export
readAnEn <- function(file.analogs, file.similarity = NULL, verbose = F, split = 0) {
  
  if (!requireNamespace('ncdf4', quietly = T)) {
    stop(paste('Please install ncdf4.'))
  }
  
  AnEn <- list()
  class(AnEn) <- 'AnEn'
  
  if (split == 0) {
    # If split is 0, no chunks will be created.
    
    if (file.exists(file.analogs)) {
      if (verbose) cat('Reading analogs from', file.analogs, '\n')
      
      nc <- ncdf4::nc_open(file.analogs)
      AnEn$analogs <- ncdf4::ncvar_get(nc, 'Analogs')
      ncdf4::nc_close(nc)
      garbage <- gc(verbose = F, reset = T)
      
      if (verbose) cat('Converting analog format ...\n')
      AnEn <- AnEnC2R(AnEn, 'analogs')
      garbage <- gc(verbose = F, reset = T)
      
    } else {
      stop(paste(file.analogs, 'does not exists.'))
    }
    
    if (!is.null(file.similarity)) {
      if (file.exists(file.similarity)) {
        if (verbose) cat('Reading similarity from', file.similarity, '...\n')
        
        nc <- ncdf4::nc_open(file.similarity)
        AnEn$similarity <- ncdf4::ncvar_get(nc, 'SimilarityMatrices')
        ncdf4::nc_close(nc)
        garbage <- gc(verbose = F, reset = T)
        
        if (verbose) cat('Converting similairty format ...\n')
        AnEn <- AnEnC2R(AnEn, 'similarity')
        garbage <- gc(verbose = F, reset = T)
        
      } else {
        stop(paste(file.similarity, 'does not exists.'))
      }
    }
    
    rm(garbage)
    
  } else {
    # Otherwise, read file in chunks
    
    file.to.read <- c()
    member.to.read <- c()
    member.to.query <- c()
    
    if (file.exists(file.analogs)) {
      file.to.read <- c(file.to.read, file.analogs)
      member.to.read <- c(member.to.read, 'analogs')
      member.to.query <- c(member.to.query, 'Analogs')
    }
    
    if (!is.null(file.similarity)) {
      if (file.exists(file.similarity)) {
        file.to.read <- c(file.to.read, file.similarity)
        member.to.read <- c(member.to.read, 'similarity')
        member.to.query <- c(member.to.query, 'Similarity')
      }
    }
    
    for (i.member in 1:length(member.to.read)) {
      if (verbose) cat(member.to.query,'input', file.to.read[i.member], '\n')
      
      nc <- ncdf4::nc_open(file.to.read[i.member])
      
      # Determine the dimension
      dims <- sapply(1:nc$var[[member.to.query[i.member]]]$ndims,
                     function(x){nc$var[[member.to.query[i.member]]]$dim[[x]]$len})
      i.max <- order(dims, decreasing = T)[1]
      indices <- round(seq(from = 1, to = dims[i.max], length.out = split + 1))
      
      if (verbose) cat('Allocating memory for an array with the dimension [',
                       paste(dims, collapse = ','), ']\n')
      AnEn[[member.to.read[i.member]]] <- array(NA, dim = dims)
      
      # Temporary container for each chunk
      AnEn.chunk <- list()
      class(AnEn.chunk) <- 'AnEn'
      
      for (i in 1:split) {
        if (verbose) cat('Reading chunk #', i, '[', indices[i], ',',
                         indices[i+1], '] out of', split, '\n')
        start <- rep(1, length(dims))
        start[i.max] <- indices[i]
        count <- dims
        count[i.max] <- indices[i+1] - indices[i] + 1
        
        AnEn.chunk$analogs <- ncdf4::ncvar_get(nc, member.to.query[i.member],
                                               start = start, count = count,
                                               collapse_degen = F)
        garbage <- gc(verbose = F, reset = T)
        
        if (verbose) cat('Converting array format ...\n')
        AnEn.chunk <- AnEnC2R(AnEn.chunk, member.to.read[i.member])
        
        AnEn[[member.to.read[i.member]]][
          start[1]:(start[1]+count[1]-1),
          start[2]:(start[2]+count[2]-1),
          start[3]:(start[3]+count[3]-1),
          start[4]:(start[4]+count[4]-1),
          start[5]:(start[5]+count[5]-1)] <-
          AnEn.chunk[[member.to.read[i.member]]]
        
        garbage <- gc(verbose = F, reset = T)
      }
      
      ncdf4::nc_close(nc)
    }
    
    rm(AnEn.chunk, garbage, member.to.query, member.to.read, file.to.read)
    garbage <- gc(verbose = F, reset = T)
  }
  
  if (verbose) cat('Done (readAnEn)!\n')
  
  return (AnEn)
}
