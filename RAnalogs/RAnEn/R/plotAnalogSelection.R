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

#' RAnEn::plotAnalogSelection
#' 
#' RAnEn::plotAnalogSelection visualize the selection of analog ensemble members.
#' It shows how ensemble members are selected based on the similarity metrics.
#' It also shows the selected forecasts for each parameter used during the 
#' analog generation, compared with the other unselected forecasts.
#' 
#' @param AnEn An AnEn object generated from \code{\link{generateAnalogs}}.
#' @param config A Configuration object generated from \code{\link{generateConfiguration}}.
#' @param i.station The station index from analogs.
#' @param i.test.day The test day index from analogs.
#' @param i.flt The FLT index from analogs.
#' @param parameter.names The parameter names that are associated with forecasts in the configuration.
#' @param cex.lab The font size of labels.
#' @param pch.selected The point type for selected points.
#' @param pch.unselected The point type for unselected points.
#' @param col.selected The color for selected points.
#' @param col.unselected The color for unselected points.
#' @param col.reference The color for the reference line.
#' @param lty.reference The line type for the reference line.
#' @param lwd.reference The line width for the reference line.
#' @param origin The origin for times in forecasts. This will be passed to as.POSIXct to convert 
#' times in forecasts to date/time objects.
#' @param tz The time zone in forecasts. This will be passed to as.POSIXct to convert 
#' times in forecasts to date/time objects.
#' @param single.figure Whether to generate only a single figure.
#' @param mar Margins for subplots. This is effective only when single.figure is TURE.
#' It is passed to the function par.
#' @param omi Outer margin for the plot. This is effective only when single.figure is TURE.
#' It is passed to the function par.
#' 
#' @author Weiming Hu \email{cervone@@psu.edu}
#' @author Laura CLemente-Harding \email{lec170@@psu.edu}
#' @author Martina Calovi \email{mcalovi@@psu.edu}
#' @author Guido Cervone \email{cervone@@psu.edu}
#' 
#' @export
plotAnalogSelection <- function(
  AnEn, config, i.station, i.test.day, i.flt, parameter.names,
  cex.lab = 1.5, pch.selected = 16, pch.unselected = 16,
  col.selected = 'red', col.unselected = 'lightgrey', col.reference = 'black',
  lty.reference = 'longdash', lwd.reference = 2, origin = '1970-01-01', tz = 'UTC',
  single.figure = T, mar = c(1.5, 5, 1, 1) + 0.1, omi = c(.3, 0, 0, 0), use.plotly = F) {
  
  # Some of the cases that I don't deal with for now
  stopifnot(!config$quick)
  stopifnot(!config$advanced)
  stopifnot(class(AnEn) == 'AnEn')
  stopifnot(class(config) == 'Configuration')
  
  # Check input parameters
  if (length(parameter.names) != dim(config$forecasts)[1]) {
    cat("Error: Parameter names and the first dimension of configuration forecasts do not match.")
  }
  if (use.plotly) {
    require(plotly)
  }
  
  # Remove the parameters with weight equals to 0
  parameter.names.used <- parameter.names[which(config$weights != 0)]
  
  # Get the similarity matrix of this particular station, test day, and flt
  sims <- AnEn$similarity[i.station, i.test.day, i.flt, , ]
  
  # Remove NA rows
  sims <- sims[which(!is.na(sims[, 1])), ]
  
  # Get the start and end index for searching in the forecasts
  search.start <- which(config$forecast_times == config$search_times_compare[1])
  search.end <- which(config$forecast_times == config$search_times_compare[
    length(config$search_times_compare)])
  
  # Generate the x axis. The x axis will be the search days
  x.days <- as.POSIXct(config$forecast_times[search.start:search.end],
                       origin = origin, tz = tz)
  
  # This is the day index associated with each row in the similarity matrix.
  # Note this is sorted by the similarity values.
  #
  days.index <- sims[, 3] - search.start + 1
  
  if (use.plotly) {
    
    if (single.figure) {
      plotly.list <- list()
    }
    
    p <- plot_ly(x = x.days[days.index], y = sims[, 1], type = 'scatter', name = "Similarity",
                 mode = 'markers', marker = list(color = col.unselected, symbol= pch.unselected)) %>%
      add_markers(x = x.days[days.index[1:config$num_members]],
                  y = sims[1:config$num_members, 1], name = "Selected",
                  marker = list(color = col.selected, symbol= pch.selected)) %>%
      layout(yaxis = list(title = 'Similarity', titlefont = cex.lab, showspikes = T),
             xaxis = list(showspikes = T), showlegend = F, hovermode = 'closest')
    
    if (single.figure) {
      plotly.list <- c(plotly.list, list(p))
    } else {
      print(p)
    }
    
    for (index in 1:length(parameter.names.used)) {
      i.parameter <- which(parameter.names.used[index] == parameter.names)
      
      current.forecast.value <- config$forecasts[
        i.parameter, i.station, (i.test.day+test.start-1), i.flt]
      
      # Extract the values for the forecasts for this particular parameter
      forecast.values <- config$forecasts[i.parameter, i.station, search.start:search.end, i.flt]
      
      # A sanity check
      stopifnot(length(x.days) == length(forecast.values))
      
      p <- plot_ly(type = 'scatter', mode = 'markers+lines') %>%
        add_markers(x = x.days, y = forecast.values, name = parameter.names[i.parameter],
                    marker = list(color = col.unselected, symbol= pch.unselected)) %>%
        add_markers(x = x.days[days.index[1:config$num_members]],
                    y = forecast.values[days.index[1:config$num_members]], name = 'Selected',
                    marker = list(color = col.selected, symbol= pch.selected)) %>%
        add_segments(x = x.days[1], xend = x.days[length(x.days)],
                     y = current.forecast.value, yend = current.forecast.value,
                     name = 'Current forecast', line = list(
                       color = col.reference, width = lwd.reference, dash = lty.reference)) %>%
        layout(yaxis = list(title = parameter.names[i.parameter], titlefont = cex.lab, showspikes = T),
               xaxis = list(showspikes = T), showlegend = F, hovermode = 'closest')
      
      if (single.figure) {
        plotly.list <- c(plotly.list, list(p))
      } else {
        print(p)
      }
    }
    
    p <- subplot(plotly.list, nrows = length(plotly.list), titleY = T, shareX = T)
    print(p)
    
  } else {
    
    if (single.figure) {
      # If you want to plot all figures in a single plot
      par(mfrow = c(length(parameter.names.used) + 1, 1), mar = mar, omi = omi)
    }
    
    # Plot the similairty and the selected ones
    plot(x.days[days.index], sims[, 1], xlab = '', ylab = 'Similarity',
         pch = pch.unselected, col = col.unselected, cex.lab = cex.lab)
    points(x.days[days.index[1:config$num_members]],
           sims[1:config$num_members, 1], pch = pch.selected, col = col.selected)
    
    for (index in 1:length(parameter.names.used)) {
      i.parameter <- which(parameter.names.used[index] == parameter.names)
      
      current.forecast.value <- config$forecasts[
        i.parameter, i.station, (i.test.day+test.start-1), i.flt]
      
      # Extract the values for the forecasts for this particular parameter
      forecast.values <- config$forecasts[i.parameter, i.station, search.start:search.end, i.flt]
      
      # A sanity check
      stopifnot(length(x.days) == length(forecast.values))
      
      plot(x.days, forecast.values, pch = pch.unselected, col = col.unselected,
           xlab = '', ylab = parameter.names[i.parameter], cex.lab = cex.lab)
      points(x.days[days.index[1:config$num_members]], forecast.values[days.index[1:config$num_members]],
             pch = pch.selected, col = col.selected)
      abline(h = current.forecast.value, col = col.reference, lty = lty.reference, lwd = lwd.reference)
    }
  }
}