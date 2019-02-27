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

#' RAnEn::plotAnalogSample
#' 
#' RAnEn::plotAnalogSample compares the current forecasts and AnEn with observations
#' for one particular station and test time across all FLTs available in forecasts.
#' It also plots the historical forecasts that are identified by AnEn, and 
#' compares them with current forecasts for each parameters used to generate
#' analogs.
#' 
#' 
#' @param AnEn An AnEn object generated from \code{\link{generateAnalogs}}.
#' @param config A Configuration object generated from \code{\link{generateConfiguration}}.
#' @param i.station The station index from analogs.
#' @param i.test.day The test day index from analogs.
#' @param obs.id The index in observation parameters that which parameter is predicted.
#' @param fcst.id The index in forecast parameters that which parameter is predicted.
#' @param parameter.names The forecast parameter names.
#' @param share.parameter.names Whether forecasts and observations have the same
#' parameters. If this is TRUE, the observations will be plot on the other parameters
#' that are used to compute the simialrity.
#' @param member.name Which member from AnEn should be plotted. This is useful
#' when you have bias corrected analog members in AnEn that you would like to plot.
#' @param flts.fraction This is the fraction when dividing FLT values. 3600 means that
#' the x labels are in hours when FLTs are in seconds.
#' @param boxplot.outliers Whether to plot outliers.
#' @param border.anen.box The color of the box plots.
#' @param lty.current.fcsts The line type for current forecasts.
#' @param col.current.fcsts The color for current forecasts.
#' @param cex.current.fcst The size for current forecasts.
#' @param pch.current.obs The point type for current observations.
#' @param col.current.obs The color for current observations.
#' @param cex.current.obs The size for current observations.
#' @param cex.label The font size for x, y, and the legend labels.
#' @param legend.texts A vector of characters of length 2. The first one
#' is for observations, and the second one for forecasts.
#' @param legend.position The legend position.
#' @param single.figure Whether to generate a single plot with subplots.
#' @param mar Margin for the plot. This is effective only when single.figure is TURE.
#' @param omi Outer margin for the plot. This is effective only when single.figure is TURE.
#' 
#' @author Weiming Hu \email{cervone@@psu.edu}
#' @author Laura CLemente-Harding \email{lec170@@psu.edu}
#' @author Martina Calovi \email{mcalovi@@psu.edu}
#' @author Guido Cervone \email{cervone@@psu.edu}
#' 
#' @md
#' @export
plotAnalogSample <- function(
  AnEn, config,
  i.station,
  i.test.day,
  obs.id,
  fcst.id,
  parameter.names,
  share.parameter.names = F,
  member.name = 'analogs',
  
  flts.fraction = 3600,
  boxplot.outliers = F,
  border.anen.box = 'grey',
  lty.current.fcsts = 'longdash',
  col.current.fcsts = 'red',
  cex.current.fcst = 1,
  pch.current.obs = 1,
  col.current.obs = 'black',
  cex.current.obs = 1.5,
  
  cex.label = 1.5,
  legend.texts = c("Observations", "NAM"),
  legend.position = 'topleft',
  
  single.figure = T,
  mar = c(1.5, 5, 1, 1) + 0.1,
  omi = c(.3, 0, 0, 0)) {
  # Check input parameters
  stopifnot(member.name %in% names(AnEn))
  stopifnot(config$preserve_similarity)
  stopifnot(config$mode == 'independentSearch')
  stopifnot(class(AnEn) == 'AnEn')
  stopifnot(class(config) == 'Configuration')
  
  if (length(parameter.names) != dim(config$forecasts)[1]) {
    stop("Parameter names and the first dimension of configuration forecasts do not match.")
  }
  
  if (dim(AnEn$analogs)[3] == 1) {
    cat("Warning: Plots might display unexpectedly because you only have 1 FLT.\n")
  }
  
  # Remove the parameters with weight equals to 0
  parameter.names.used <- parameter.names[which(config$weights != 0)]
  
  # Get the start and end index for searching in the forecasts
  test.start <- which(config$forecast_times == config$test_times_compare[1])
  
  # Get the AnEn forecasts for the test day and the station
  values.anen <- AnEn[[member.name]][i.station, i.test.day, , , 1]
  
  # Get the current forecasts
  values.fcsts <- config$forecasts[fcst.id, i.station, (i.test.day+test.start-1), ]
  
  # Get the observations for current forecasts. These are the values that really happen for 
  # for this test day and test station.
  #
  time.index.obs <- sapply(config$forecast_times[i.test.day+test.start-1] + config$flts,
                           function(x) {which(x == config$observation_times)})
  stopifnot(length(time.index.obs) == length(values.fcsts))
  values.obs <- config$search_observations[obs.id, i.station, time.index.obs]
  
  # Get the index for historical most similar forecasts
  day.index.similar.forecasts <- AnEn$similarity[i.station, i.test.day, , 1:config$num_members, 3]
  
  # Remove the negative values because they are only for debugging purposes
  day.index.similar.forecasts[which(day.index.similar.forecasts<0)] <- NA
  
  # This is our x axis ticks and labels
  xs <- config$flts / flts.fraction
  
  if (single.figure) {
    # If you want to plot all figures in a single plot
    par(mfrow = c(length(parameter.names.used) + 1, 1), mar = mar, omi = omi)
  }
  
  ylim <- range(values.anen, values.fcsts, values.obs, na.rm = T)
  
  boxplot(t(values.anen), names = xs, at = xs, outline = boxplot.outliers, cex.lab = cex.label,
          border = border.anen.box, ylab = parameter.names[fcst.id], ylim = ylim)
  lines(xs, values.fcsts, lty = lty.current.fcsts, col = col.current.fcsts,
        cex = cex.current.fcst, type = 'b')
  points(xs, values.obs, pch = pch.current.obs, col = col.current.obs,
         cex = cex.current.obs)
  
  legend(legend.position, legend = legend.texts, cex = cex.label,
         pch = c(pch.current.obs, NA), lty = c(NA, lty.current.fcsts),
         col = c(col.current.obs, col.current.fcsts))
  
  for (index in 1:length(parameter.names.used)) {
    i.par <- which(parameter.names.used[index] == parameter.names)
    
    values.par.fcsts <- config$forecasts[i.par, i.station, (test.start+i.test.day-1), ]
    complex.index <- cbind(i.par, i.station, as.vector(day.index.similar.forecasts),
                           rep(1:dim(config$forecasts)[4], times = config$num_members))
    values.similar.forecasts <- matrix(
      ncol = dim(day.index.similar.forecasts)[2],
      data = config$forecasts[complex.index])
    
    ylim <- range(values.similar.forecasts, values.par.fcsts, na.rm = T)
    
    boxplot(t(values.similar.forecasts), at = xs, names = xs, ylim = ylim,
            outline = boxplot.outliers, border = border.anen.box,
            ylab = parameter.names[i.par], cex.lab = cex.label)
    lines(xs, values.par.fcsts, lty = lty.current.fcsts, type = 'b',
          col = col.current.fcsts, cex = cex.current.fcst)
    
    if (share.parameter.names) {
      values.obs <- config$search_observations[i.par, i.station, time.index.obs]
      points(xs, values.obs, pch = pch.current.obs,
             col = col.current.obs, cex = cex.current.obs)
    }
  }
}
