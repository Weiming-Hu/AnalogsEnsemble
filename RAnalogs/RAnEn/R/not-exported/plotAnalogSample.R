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
#' @param analogs The generated analogs with 4 dimensions.
#' @param config A Configuration object generated from \code{\link{generateConfiguration}}.
#' @param i.station The station index from analogs.
#' @param i.test.day The test day index from analogs.
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
  analogs, sims, sims.index,
  config,
  i.station,
  i.test.day,
  fcst.id,
  parameter.names,
  share.parameter.names = F,
  
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
  
  # Get configuration names
  config.names <- getConfigNames()
  
  # Check input parameters
  stopifnot(class(config) == 'Configuration')
  stopifnot(config$mode == 'independentSearch')
  
  if (length(parameter.names) != dim(config[[config.names$`_FCSTS`]])[1]) {
    stop("Parameter names and the first dimension of configuration forecasts do not match.")
  }
  
  if (dim(analogs)[3] == 1) {
    cat("Warning: Plots might display unexpectedly because you only have 1 FLT.\n")
  }
  
  # Remove the parameters with weight equals to 0
  if (length(config[[config.names$`_WEIGHTS`]]) == 0) {
    parameter.names.used <- parameter.names
  } else {
    parameter.names.used <- parameter.names[which(config[[config.names$`_WEIGHTS`]] != 0)]
  }
  
  # Get the start and end index for searching in the forecasts
  test.start <- which(config[[config.names$`_FCST_TIMES`]] == config[[config.names$`_TEST_TIMES`]][1])
  
  # Get the AnEn forecasts for the test day and the station
  values.anen <- analogs[i.station, i.test.day, , ]
  
  # Get the current forecasts
  values.fcsts <- config[[config.names$`_FCSTS`]][fcst.id, i.station, (i.test.day+test.start-1), ]
  
  # Get the observations for current forecasts. These are the values that really happen for 
  # for this test day and test station.
  #
  time.index.obs <- sapply(config[[config.names$`_FCST_TIMES`]][i.test.day+test.start-1] + config[[config.names$`_FLTS`]], function(x) {
    i <- which(x == config[[config.names$`_OBS_TIMES`]])
    if (length(i) == 1) return(i)
    else return(NA)})
  stopifnot(length(time.index.obs) == length(values.fcsts))
  values.obs <- config[[config.names$`_OBS`]][config[[config.names$`_OBS_ID`]], i.station, time.index.obs]
  
  # Get the index for historical most similar forecasts
  day.index.similar.forecasts <- sims.index[i.station, i.test.day, , 1:config[[config.names$`_NUM_MEMBERS`]]]
  
  # Remove the indices when the corresponding similarity is NA
  day.index.similar.forecasts[which(is.na(
    sims[i.station, i.test.day, , 1:config[[config.names$`_NUM_MEMBERS`]]]))] <- NA
  
  # Remove the negative values because they are only for debugging purposes
  negative.pos <- which(day.index.similar.forecasts<0)
  if (length(negative.pos) != 0) {
    day.index.similar.forecasts[negative.pos] <- NA
  }
  
  # This is our x axis ticks and labels
  xs <- config[[config.names$`_FLTS`]] / flts.fraction
  
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
    
    values.par.fcsts <- config[[config.names$`_FCSTS`]][i.par, i.station, (test.start+i.test.day-1), ]
    complex.index <- cbind(i.par, i.station, as.vector(day.index.similar.forecasts),
                           rep(1:dim(config[[config.names$`_FCSTS`]])[4], times = config[[config.names$`_NUM_MEMBERS`]]))
    values.similar.forecasts <- matrix(
      ncol = dim(day.index.similar.forecasts)[2],
      data = config[[config.names$`_FCSTS`]][complex.index])
    
    ylim <- range(values.similar.forecasts, values.par.fcsts, na.rm = T)
    
    boxplot(t(values.similar.forecasts), at = xs, names = xs, ylim = ylim,
            outline = boxplot.outliers, border = border.anen.box,
            ylab = parameter.names[i.par], cex.lab = cex.label)
    lines(xs, values.par.fcsts, lty = lty.current.fcsts, type = 'b',
          col = col.current.fcsts, cex = cex.current.fcst)
    
    if (share.parameter.names) {
      values.obs <- config[[config.names$`_OBS`]][i.par, i.station, time.index.obs]
      points(xs, values.obs, pch = pch.current.obs,
             col = col.current.obs, cex = cex.current.obs)
    }
  }
}
