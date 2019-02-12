---
layout: post
title: Search Space Extension with RAnEn
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

This article demonstrate how to use the `RAnEn` package with the search extension functionality. If you haven't done so, please read [the instructions for basic usage of `RAnEn`](https://weiming-hu.github.io/AnalogsEnsemble/2018/11/04/demo-1-RAnEn-basics.html) first. This article skips the part that has been covered in the previous article.

The classic `AnEn` algorithm searches for the most similar historical foreasts at its current location. Therefore, only forecasts from the current station/grid point will be traversed and compared. This search style is referred to as the *independent search*. Another possible search style is *extended search*, which is also referred to as *search space extension*. It simply indicates that forecasts at nearby stations/grid points are included in the search process. As a result, the search space is significantly larger by using the search space extension.

There are currently two ways to define what nearby locations to be included into the search. Users can set the nearest number of neighbors to be included or a distance threshold. Two restraints can be used together.

You will learn how to use these functions:

-   `generateAnalogs`

Data Description
----------------

We reuse the data from the [demo 1 AnEn Basics](https://weiming-hu.github.io/AnalogsEnsemble/2018/11/04/demo-1-RAnEn-basics.html). Please refer to the article for more detailed data description. The message shows the name of variables loaded.

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

Generate Temperature Forecasts
------------------------------

It is similar to generate AnEn forecasts with search space extension functionaltiy. First, we create a configuration and set up some common parameters to what we have done for independent search. **Please note that we specify the type of configuration when we create it**.

If you don't understand any of the options, please see the [document](https://weiming-hu.github.io/AnalogsEnsemble/R/reference/generateConfiguration.html).

Now that we have set up the basic parameters, we need to set up the parameters for search space extension. A list of parameters that should be set is shown below:

-   test\_stations\_x
-   test\_stations\_y
-   search\_stations\_x
-   search\_stations\_y
-   num\_nearest
-   distance
-   max\_num\_search\_stations
-   preserve\_search\_stations

The first four parameters specify the locations of search and test stations/grid points. The unit is ambiguous unless it is consistent. It is user defined. It can be meters or lat/lon unless it remains the same for all four parameters. No projection is imposed by the package. Therefore any geographic projection should be done prior to the AnEn computation. `num_nearest` is the number of neighbors that should be included into the search space. `distance` is the threshold to include nearby stations. Similarly, the unit of the distance should be consistent with locations. `max_num_search_stations` is an estimated maximum number of nearby stations that could possibily be found. If you are using `num_nearest`, it is a good practice to set `max_num_search_stations` to the same as `num_nearest`. If you are only using `distance`, there is a possibility that too many nearby stations/grid points are found within this distance and there is not enough memory to store all the search data. User should be responsible to provide an upper limit for how many stations should be included for search. In this sense, `max_num_search_stations` is similar to `num_nearest` but `max_num_search_stations` is an optimization parameter and `num_nearest` is a utility parameter. The last parameter `preserve_search_stations` indicates whether to save the search stations in the returned result.

Once we understand the parameters, we set these parameters.

Similarly we can generate AnEn and bias correct the AnEn.

``` r
AnEn <- generateAnalogs(config)
```

    ## Convert R objects to C++ objects ...
    ## Wrapping C++ object mapping ...
    ## Wrapping C++ object search stations ...
    ## Wrapping C++ object analogs ...
    ## Done!

``` r
AnEn <- biasCorrection(AnEn, config, forecast.ID = 1, group.func = mean,
                       na.rm = T, show.progress = F)
```

By now we have our AnEn forecasts. Note that we have the search stations stored in the returned result.

``` r
print(AnEn)
```

    ## Class: AnEn list
    ## Member 'analogs': [test station][test time][FLT][member][type]
    ## 121 1 36 19 3 (value, search station, search observation time)
    ## Member 'mapping': [FLT][forecast time] 36 370
    ## Member 'searchStations': [search station][test station] 15 121
    ## Member 'analogs.cor.insitu': [test station][test time][FLT][member][type]
    ## 121 1 36 19 3 (value, search station, search observation time)

Visualization
-------------

Finally, we can make some plots.

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

![Result comparison](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2018-11-24-search-extension/unnamed-chunk-6-1.png)

We can also visualize the test and search stations.

``` r
# Select a test location of which the search locations will be shown.
test.station.index <- 41
day.index <- 1
flt.index <- 1

# Parameters for plot
cex <- 1
offset <- 10
label.size <- .8
title <- 'Search Space Demo'

# Get the selected stations from ensemble members
selected.stations <- unique(
  AnEn$analogs[test.station.index, day.index, flt.index, , 2])

# plot different types of stations together
par(mfrow = c(1, 1))
plot(config$search_stations_x, config$search_stations_y, xlab = 'x', ylab = 'y',
     main = title, pch = 19, cex = cex, col = 'grey', asp=1)
map(col = 'grey', add = T); map('state', add = T)
points(config$search_stations_x[AnEn$searchStations[, test.station.index]],
       config$search_stations_y[AnEn$searchStations[, test.station.index]],
       pch = 19, cex = cex, col = 'green')
points(config$test_stations_x[test.station.index],
       config$test_stations_y[test.station.index],
       pch = 19, cex = cex/2, col = 'red')
points(config$search_stations_x[selected.stations],
       config$search_stations_y[selected.stations],
       cex = cex*1.6, col = 'red')


if (config$distance > 0) {
  coords <- generateCircleCoords(
    x = config$test_stations_x[test.station.index],
    y = config$test_stations_y[test.station.index],
    radius = config$distance, np = 100)
  
  points(coords, cex = .1, pch = 19)
}

legend('top', legend = c('grid point', 'search', 'test', 'selected'),
       pch = c(19, 19, 19 , 1), col = c('grey', 'green', 'red', 'red'), horiz = T)
```

![Geographic plot](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2018-11-24-search-extension/unnamed-chunk-7-1.png)
