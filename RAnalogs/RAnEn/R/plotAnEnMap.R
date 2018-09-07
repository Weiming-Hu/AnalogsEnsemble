# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Guido Cervone <cervone@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::plotAnEnMap
#' 
#' RAnEn::plotAnEnMap documentation underdevelopment.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' 
#' @export
plotAnEnMap  <- function(
  ras, z = "obs", spdf = NULL, spdf.bg = NULL, spdf.coastline = NULL, 
  col = rainbow(9), col.coastline = "darkgrey", col.bg = "white", col.text = "black",
  col.interpolate = T, border = "darkgrey", xlab = "Longitude", ylab = "Latitude",
  lwd = 1, cex = 1, lwd.coastline = 1, lwd.bg = 1, smallplot = c(.1, .14, .6, .90),
  legend = "", legend.line = 1, legend.adj = 0, add.legend = T, zlim = NULL, ext = NULL) {
  
  par(mar=c(2,2,0,0))
  
  par('fg'=  col.text)
  par('col.axis'=  col.text)
  
  if (is.null(zlim)) {
    zlim <- range(values(ras),na.rm=T)
  }
  
  if (is.null(ext)) {
    ext <- extent(ras)
  }
  
  xmin <- ext[1]
  xmax <- ext[2]
  ymin <- ext[3]
  ymax <- ext[4]
  
  at      <-  pretty(zlim)
  rat     <-  range(at)
  ll      <-  length(rat[1]:rat[2])
  
  if (col.interpolate) {
    col.int <-  colorRampPalette( col )(ll)  # Interpolate to generate more colors
  } else {
    col.int <- col
  }
  
  plot(extent(ras),xaxs="i",yaxs="i",xlab=xlab, ylab=ylab, xlim=c(xmin,xmax), ylim=c(ymin,ymax))
  plot(ras,col=col.int,zlim=zlim,legend=F,add=T)
  
  
  if (!is.null(spdf.bg)) {
    plot(spdf.bg,border=col.bg, add=T,lwd=lwd.bg)
  }
  
  
  if( !is.null(spdf)) {
    values <- spdf[[eval(z)]]
    
    if ( class(spdf) =="SpatialPolygonsDataFrame" ) {
      plot(spdf,border=border,col=val2col(x=values,min=zlim[1], max=zlim[2], col=col.int),add=T,lwd=lwd)
    } else {
      plot(spdf,pch=21,col=border,bg=val2col(x=values,min=zlim[1], max=zlim[2], col=col.int),add=T,lwd=lwd, cex=cex)
    }
  }
  
  if (!is.null(spdf.coastline)) {
    plot(spdf.coastline,add=T,border=col.coastline,lwd=lwd.coastline)
  }
  
  if ( add.legend ) {
    plot(ras,legend.only=T,border=NA,
         legend.arg=list(text=legend,line=legend.line,adj=legend.adj), 
         axis.arg=list(at=at, labels=at, cex.axis=1),
         smallplot=smallplot,col=col.int,zlim=range(at))
  }  
  
  box()
}