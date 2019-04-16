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

#' RAnEn::plotAnEnTimesSeries
#' 
#' RAnEn::plotAnEnTimesSeries documentation underdevelopment.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' 
#' @export
plotAnEnTimesSeries <- function(
  analogs, mod, obs, col.frame = "grey", col.bg = "lightgrey",
  col.lines = "bisque4", ylim=NULL, n=10) {
  
  # Let's plot the temporal downscaling
  #
  # No bias correction apllied
  
  xs    <- analogs$x
  bx    <- boxplot(t(analogs$y),outline=F,at=xs,plot=F)
  
  cols  <-  brewer.pal(3,name="Blues")
  
  
  #at    <- xs[seq(1,length(xs),length=length(mod$x))]
  at     <- mod$x
  labs  <- format(as.POSIXct(at,origin="1970-01-01",tz='UTC'),"%H:%M")
  days  <- unique(format(as.POSIXct(at,origin="1970-01-01",tz='UTC'),"%Y-%m-%d"))
  
  
  
  
  par(mar=c(5,4,1,1))
  
  if (is.null(ylim)) {ylim=range(bx$stats,na.rm=T)}
  
  #ylim[1] = sign(ylim[1]) * ( abs(ylim[1])*1.5)
  #
  plot(xs, xs, ylim=ylim,type="n",axes=F,ylab="Temp (C)",xlab="")
  mtext("Time (HH:MM)",1,line=4)
  rect(-1E10,-1E5,1E10,1E5,col=col.bg,border=NA)
  grid(NA,6,col='white')
  
  polygon(c(xs,rev(xs)), c( runningAverage(bx$stats[1,],n),
                            runningAverage(rev(bx$stats[5,]),n) ),col=cols[1],border=NA )
  polygon(c(xs,rev(xs)), c( runningAverage(bx$stats[2,],n),
                            runningAverage(rev(bx$stats[4,]),n) ),col=cols[2],border=NA )
  
  
  daysec <- 24*60*60
  lines.lwd = 3 
  
  
  day1 <- as.POSIXct(days[1],origin="1970-01-01",tz='UTC')
  lines(rep( day1,2),c(-100,100),lty=1,lwd=lines.lwd,col=col.lines)
  text(day1+daysec/2,ylim[1],day1,col=col.lines )
  
  day2 <- as.POSIXct(days[1],origin="1970-01-01",tz='UTC')+daysec
  lines(rep( day2,2),c(-100,100),lty=1,lwd=lines.lwd,col=col.lines)
  text(day2+daysec/2,ylim[1],day2,col=col.lines )
  
  day3 <- as.POSIXct(days[1],origin="1970-01-01",tz='UTC')+2*daysec
  lines(rep( day3,2),c(-100,100),lty=1,lwd=lines.lwd,col=col.lines)
  text(day3+daysec/2,ylim[1],day3,col=col.lines )
  
  day4 <- as.POSIXct(days[1],origin="1970-01-01",tz='UTC')+3*daysec
  lines(rep( day4,2),c(-100,100),lty=1,lwd=lines.lwd,col=col.lines)
  
  
  # # Plot the ensemble mean
  lines(xs, runningAverage(bx$stats[3,],n),col=cols[3])
  points(xs, runningAverage(bx$stats[3,],n),col=cols[3],pch=19,cex=.5)
  
  # Plot the average observations for the stations
  lines(obs$x, obs$y, col="green4",lty=2)
  points(obs$x, obs$y, col="green4",pch=19,cex=.5)
  
  
  # Plot GFS
  
  lines.step( mod, col="red4",lty=1)
  lines( mod, col="red4",lty=3)
  points( mod, col="red4",pch=19)
  text(mod$x,mod$y,1:length(mod$x),col="red4",pos=1,adj=c(.5,.5))
  
  
  #axis(1,at=c(day1,day2,day3,day4),labels=F,col=NA,col.ticks=col.lines,lwd=lines.lwd,tck=-.04) # The tickmarks for the 00:00
  axis(1,at=at,labels=labs,las=2, col=col.frame)
  axis(2, col=col.frame)
  box(col=col.frame)
  
  leg <- c("Mod","Obs","AnEn","M1","M2")
  legend('top',horiz='T',legend=leg, border=NA,text.width =  strwidth(leg) *1.5,
         fill=c("red4","green4",cols[3],cols[2],cols[1]),bty="o",bg=col.bg,box.col=col.bg)
}