---
layout: post
title: Operational Search with RAnEn
tags:
  - tutorial
---

<!-- vim-markdown-toc GitLab -->

* [Introduction](#introduction)
* [Data Description](#data-description)
* [Generate Temperature Forecasts](#generate-temperature-forecasts)
* [Visualization](#visualization)

<!-- vim-markdown-toc -->

Introduction
------------

The prediction accuracy of Analog Ensemble depends on the quality of analogs. Presumably, better analogs will generate better predictions. In real-time model operation, when historical forecasts increasing and the test forecast moving forward in time, it is most likely that the historical forecasts in the near past are the most similar ones. Therefore, in an operational prediction mode, it is desired to increment the search forecasts when test forecasts are moving towards the future.

This article shows an example of how to use `RAnEn` with an operational search. It is strongly suggested to go over the [demo 1](https://weiming-hu.github.io/AnalogsEnsemble/2018/11/04/demo-1-RAnEn-basics.html) prior to this tutorial.

Data Description
----------------

Same data are used from the [demo 1](https://weiming-hu.github.io/AnalogsEnsemble/2018/11/04/demo-1-RAnEn-basics.html).

``` r
load('analysis.RData')
load('forecasts.RData')

# Only use one parameter, the surface temperature
forecasts <- forecasts[1, , , , drop = F]

print(ls())
```

    ##  [1] "analysis"               "analysis.circular.pars"
    ##  [3] "analysis.indices"       "analysis.par.names"    
    ##  [5] "analysis.times"         "analysis.xs"           
    ##  [7] "analysis.ys"            "flts"                  
    ##  [9] "forecast.circular.pars" "forecast.indices"      
    ## [11] "forecast.par.names"     "forecast.times"        
    ## [13] "forecast.xs"            "forecast.ys"           
    ## [15] "forecasts"

Generate Temperature Forecasts
------------------------------

When generating forecasts with an operational search, we need to pay attention to two sets of configuration parameters:

-   `test_times` and `search_times`: These two parameters are the complete and the largest sets of test/search times that will be possibly generated and used. And these two parameters should be consistent with the number of times in test/search forecasts.
-   `test_times_compare` and `search_times_compare`: These two parameters are the actual test/search times that will be compared and simialrity will be generated for each combination of `test_times_compare` and `search_search_compare`. **However, if the operational search is desired, `search_search_compare` becomes meaningless because each test time will have a different set of search times, and search times will be regenerated for each test times. The generated search times will be a subset of or equal to `search_times`**.

If you don't understand any of the options, please see the [document](https://weiming-hu.github.io/AnalogsEnsemble/R/reference/generateConfiguration.html).

We then generate analogs.

``` r
AnEn.opr <- generateAnalogs(config)
```

    ## Convert R objects to C++ objects ...
    ## Wrapping C++ object similarity matrix ...
    ## Wrapping C++ object mapping ...
    ## Wrapping C++ object analogs ...
    ## Done!

``` r
AnEn.opr <- biasCorrection(AnEn.opr, config, forecast.ID = 1, group.func = mean,
                           na.rm = T, show.progress = F)
```

We generate another AnEn without operational search.

``` r
search.start <- 1

# Remove the day to avoid overlapping between search and test forecasts
search.end <- test.start - 2

config$operational <- F
config$search_times_compare <- forecast.times[search.start:search.end]
AnEn.ind <- generateAnalogs(config)
```

    ## Convert R objects to C++ objects ...
    ## Wrapping C++ object similarity matrix ...
    ## Wrapping C++ object mapping ...
    ## Wrapping C++ object analogs ...
    ## Done!

``` r
AnEn.ind <- biasCorrection(AnEn.ind, config, forecast.ID = 1, group.func = mean,
                           na.rm = T, show.progress = F)
```

Visualization
-------------

We then compare the prediction accuracy on the last day in our test times.

``` r
# Choose a FLT to plot
i.flt <- 31

# Choose the last test time
i.day <- 5

# Calculate ensemble means
mean.analogs.opr.cor <- apply(AnEn.opr$analogs.cor[, , , , 1, drop = F], c(1, 2, 3), mean, na.rm = T)
mean.analogs.ind.cor <- apply(AnEn.ind$analogs.cor[, , , , 1, drop = F], c(1, 2, 3), mean, na.rm = T)

# Create an empty raster for the geographical region
xy <- cbind(as.numeric(forecast.xs) - 360, as.numeric(forecast.ys))
colnames(xy) = c('x', 'y')
ext <- extent(xy[, c('x', 'y')])
rast <- raster(ext, nrow = 11, ncol = 11, crs = CRS("+proj=longlat +datum=WGS84"))

# Rasterize the data values
rast.anen.opr.cor <- rasterize(xy[, c('x', 'y')], rast, mean.analogs.opr.cor[, i.day, i.flt], fun = mean)
rast.anen.ind.cor <- rasterize(xy[, c('x', 'y')], rast, mean.analogs.ind.cor[, i.day, i.flt], fun = mean)
rast.fcts <- rasterize(xy[, c('x', 'y')], rast, forecasts[1, , test.start, i.flt], fun = mean)
rast.anly <- rasterize(xy[, c('x', 'y')], rast, analysis[
  1, , which(analysis.times == forecast.times[test.start] + flts[i.flt])], fun = mean)

# Calculate RMSE
rmse.fcts <- sqrt(mean((values(rast.fcts - rast.anly))^2, na.rm = T))
rmse.anen.opr.cor <- sqrt(mean((values(rast.anen.opr.cor - rast.anly))^2, na.rm = T))
rmse.anen.ind.cor <- sqrt(mean((values(rast.anen.ind.cor - rast.anly))^2, na.rm = T))

# Create a color scale with 100 colors
cols <- colorRampPalette(brewer.pal(11, 'Spectral')[11:1])(100)

# Define value limit
zlim <- range(c(values(rast.fcts), values(rast.anen.opr.cor),
                values(rast.anly), values(rast.anen.ind.cor)),
              na.rm = T)
# Visualization
par(mfrow = c(2, 2), mar = c(4, 1, 3, 3))
plot(rast.anly, main = 'NAM Analysis', col = cols, zlim = zlim, legend = F)
map(col = 'grey', add = T); map('state', add = T)
plot(rast.fcts, main = 'NAM Forecasts', col = cols, zlim = zlim,
     xlab = paste('RMSE =', round(rmse.fcts, 4)))
map(col = 'grey', add = T); map('state', add = T)
plot(rast.anen.ind.cor, main = 'AnEn Independent Search', col = cols, zlim = zlim,
     legend = F, xlab = paste('RMSE =', round(rmse.anen.ind.cor, 4)))
map(col = 'grey', add = T); map('state', add = T)
plot(rast.anen.opr.cor, main = 'AnEn Operational Search', col = cols,
     zlim = zlim, xlab = paste('RMSE =', round(rmse.anen.opr.cor, 4)))
map(col = 'grey', add = T); map('state', add = T)
```

![Result comparison](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2019-02-12-operational-search/unnamed-chunk-5-1.png)

If we take a closer look at the analog members of inependent search and operational search. The search times of independent search and the operational search for a specific test time are different. This is indeed the results from the operational search.

``` r
cat('The closest analog member time to the test time', as.character(forecast.times[test.start + i.day - 1] + flts[i.flt]), ':\n')
```

    ## The closest analog member time to the test time 2018-03-09 06:00:00 :

``` r
cat('(independent search) ', as.character(analysis.times[range(AnEn.ind$analogs[, i.day, i.flt, , 3], na.rm = T)[2]]), '\n')
```

    ## (independent search)  2018-03-03 06:00:00

``` r
cat('(operational search) ', as.character(analysis.times[range(AnEn.opr$analogs[, i.day, i.flt, , 3], na.rm = T)[2]]), '\n')
```

    ## (operational search)  2018-03-07 06:00:00
