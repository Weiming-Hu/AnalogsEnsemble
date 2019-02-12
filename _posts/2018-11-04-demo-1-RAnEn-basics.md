---
layout: post
title: Basics of RAnEn
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

This article walks you through the basic usage of the `RAnEn` library in a practice of short-term surface temperature forecasts. Data are prepared using the `generate-data.R` script, and collected from [North American Mesoscale Forecast System (NAM)](https://www.ncdc.noaa.gov/data-access/model-data/model-datasets/north-american-mesoscale-forecast-system-nam).

You will learn how to use these functions:

-   `generateConfiguration`
-   `generateAnalogs`
-   `biasCorrection`

Data Description
----------------

First, let's load the prepared NAM model forecasts and analysis.

``` r
load('analysis.RData')
load('forecasts.RData')
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

In total, 10 parameters are provided in the forecast data set.

``` r
print(forecast.par.names)
```

    ##  [1] "SurfaceTemperature"         "TropoWindU"                
    ##  [3] "TropoWindV"                 "SurfacePressure"           
    ##  [5] "TotalCloudCover"            "TotalPrecipitation"        
    ##  [7] "DownwardShortWaveRadiation" "DownwardLongWaveRadiation" 
    ##  [9] "UpwardShortWaveRadiation"   "UpwardLongWaveRadiation"

Foreacsts and analysis have the following time range.

``` r
print(range(forecast.times))
```

    ## [1] "2017-01-01 UTC" "2018-07-31 UTC"

``` r
print(range(analysis.times))
```

    ## [1] "2017-01-01 00:00:00 UTC" "2018-07-31 06:00:00 UTC"

Coordinates in lat/lon are provided for grid points. We can plot them on maps. Note that coordinates start from the bottom left grid point and proceed in a row-wise order. There are 11 rows and 11 columns of grid points in the data sets.

``` r
map('state', regions = c('pennsylvania', 'new york', 'virginia'), col = 'grey')
map('usa', add = T)
points(forecast.xs - 360, forecast.ys, pch = 16, cex = .3, col = 'red')
```

![Geography of the region of interest](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2018-11-04-demo-1-RAnEn-basics/geography.jpg)

Finally, let's look at the dimensions of forecasts and analysis.

``` r
print(dim(forecasts))
```

    ## [1]  10 121 576  36

``` r
print(dim(analysis))
```

    ## [1]   10  121 2805

Forecasts have 4 dimensions. They are parameters, stations/grid points, days, and Forecast Lead Times (FLTs). Analysis have 3 dimensions. They are parameters, stations/grid points, and time.

Generate Temperature Forecasts
------------------------------

Now that we have a general idea of the data, we can generate temperature forecasts. The steps to generate AnEn forecasts are as follow:

-   Initialize a configuration
-   Set up options
-   Generate AnEn

First, we set up the configuration. We generate AnEn for one day using about one year of historical data for all the grid points available. If you don't understand any of the options, please see the [document](https://weiming-hu.github.io/AnalogsEnsemble/R/reference/generateConfiguration.html).

Now we can generate AnEn.

``` r
AnEn <- generateAnalogs(config)
```

    ## Convert R objects to C++ objects ...
    ## Wrapping C++ object mapping ...
    ## Wrapping C++ object analogs ...
    ## Done!

Then we need to bias correct AnEn because we are using AnEn on a gridded product.

``` r
AnEn <- biasCorrection(AnEn, config, forecast.ID = 1, group.func = mean,
                       na.rm = T, show.progress = F)
```

`AnEn` is a list of results. Let's check what we have in it.

``` r
print(AnEn)
```

    ## Class: AnEn list
    ## Member 'analogs': [test station][test time][FLT][member][type]
    ## 121 1 36 19 3 (value, search station, search observation time)
    ## Member 'mapping': [FLT][forecast time] 36 370
    ## Member 'analogs.cor.insitu': [test station][test time][FLT][member][type]
    ## 121 1 36 19 3 (value, search station, search observation time)

Visualization
-------------

Finally, we can plot together the analysis, the forecasts, the AnEn, and the bias corrected AnEn. Below the figures are the respective RMSE.

``` r
# Choose a FLT to plot
i.flt <- 31

# Calculate ensemble means
mean.analogs <- apply(AnEn$analogs[, , , , 1, drop = F], c(1, 2, 3), mean, na.rm = T)
mean.analogs.cor <- apply(AnEn$analogs.cor[, , , , 1, drop = F], c(1, 2, 3), mean, na.rm = T)

# Create an empty raster for the geographical region
xy <- cbind(as.numeric(forecast.xs) - 360, as.numeric(forecast.ys))
colnames(xy) = c('x', 'y')
ext <- extent(xy[, c('x', 'y')])
rast <- raster(ext, nrow = 11, ncol = 11, crs = CRS("+proj=longlat +datum=WGS84"))

# Rasterize the data values
rast.anen <- rasterize(xy[, c('x', 'y')], rast, mean.analogs[, 1, i.flt], fun = mean)
rast.anen.cor <- rasterize(xy[, c('x', 'y')], rast, mean.analogs.cor[, 1, i.flt], fun = mean)
rast.fcts <- rasterize(xy[, c('x', 'y')], rast, forecasts[1, , test.start, i.flt], fun = mean)
rast.anly <- rasterize(xy[, c('x', 'y')], rast, analysis[
  1, , which(analysis.times == forecast.times[test.start] + flts[i.flt])], fun = mean)

# Calculate RMSE
rmse.fcts <- sqrt(mean((values(rast.fcts - rast.anly))^2, na.rm = T))
rmse.anen <- sqrt(mean((values(rast.anen - rast.anly))^2, na.rm = T))
rmse.anen.cor <- sqrt(mean((values(rast.anen.cor - rast.anly))^2, na.rm = T))

# Create a color scale with 100 colors
cols <- colorRampPalette(brewer.pal(11, 'Spectral')[11:1])(100)

# Define value limit
zlim <- range(c(values(rast.fcts), values(rast.anen),
                values(rast.anly), values(rast.anen.cor)),
              na.rm = T)
# Visualization
par(mfrow = c(2, 2), mar = c(4, 1, 3, 3))
plot(rast.anly, main = 'NAM Analysis', col = cols, zlim = zlim, legend = F)
map(col = 'grey', add = T); map('state', add = T)
plot(rast.fcts, main = 'NAM Forecasts', col = cols, zlim = zlim,
     xlab = paste('RMSE =', round(rmse.fcts, 4)))
map(col = 'grey', add = T); map('state', add = T)
plot(rast.anen, main = 'AnEn Averaged', col = cols, zlim = zlim,
     legend = F, xlab = paste('RMSE =', round(rmse.anen, 4)))
map(col = 'grey', add = T); map('state', add = T)
plot(rast.anen.cor, main = 'Bias Corrected AnEn Averaged', col = cols,
     zlim = zlim, xlab = paste('RMSE =', round(rmse.anen.cor, 4)))
map(col = 'grey', add = T); map('state', add = T)
```

![Result comparison](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2018-11-04-demo-1-RAnEn-basics/results.jpg)
