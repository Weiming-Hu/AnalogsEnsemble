---
layout: post
title: RAnEn Basics
---

<!-- vim-markdown-toc GitLab -->

* [Introduction](#introduction)
* [Data Description](#data-description)
* [Generate Temperature Forecasts](#generate-temperature-forecasts)
* [Visualization](#visualization)

<!-- vim-markdown-toc -->

## Introduction

This article walks you through the basic usage of the `RAnEn` library in
a practice of short-term surface temperature forecasts. Data are
prepared using the `generate-data.R` script from [North American
Mesoscale Forecast System
(NAM)](https://www.ncdc.noaa.gov/data-access/model-data/model-datasets/north-american-mesoscale-forecast-system-nam).

You will learn how to use these functions:

-   `generateConfiguration`
-   `generateAnalogs`
-   `biasCorrection`

Results are reproduceable on [binder](https://mybinder.org/v2/gh/Weiming-Hu/AnalogsEnsemble/master).

## Data Description

First, let’s load the prepared NAM model forecasts and analysis.

```
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

```
print(forecast.par.names)
```

    ##  [1] "SurfaceTemperature"         "TropoWindU"                
    ##  [3] "TropoWindV"                 "SurfacePressure"           
    ##  [5] "TotalCloudCover"            "TotalPrecipitation"        
    ##  [7] "DownwardShortWaveRadiation" "DownwardLongWaveRadiation" 
    ##  [9] "UpwardShortWaveRadiation"   "UpwardLongWaveRadiation"

Foreacsts and analysis have the following time range.

```
print(range(forecast.times))
```

    ## [1] "2017-01-01 UTC" "2018-07-31 UTC"

```
print(range(analysis.times))
```

    ## [1] "2017-01-01 00:00:00 UTC" "2018-07-31 06:00:00 UTC"

Coordinates in lat/lon are provided for grid points. We can plot them on
maps. Note that coordinates start from the bottom left grid point and
proceed in a row-wise order. There are 11 rows and 11 columns of grid
points in the data sets.

```
map('state', regions = c('pennsylvania', 'new york', 'virginia'), col = 'grey')
map('usa', add = T)
points(forecast.xs - 360, forecast.ys, pch = 16, cex = .3, col = 'red')
```

![Geography of the region of interest](https://github.com/Weiming-Hu/AnalogsEnsemble/blob/gh-pages/assets/posts/2018-11-04-demo-1-RAnEn-basics/geography.png)

Finally, let’s look at the dimensions of forecasts and analysis.

```
print(dim(forecasts))
```

    ## [1]  10 121 576  36

```
print(dim(analysis))
```

    ## [1]   10  121 2805

Forecasts have 4 dimensions. They are parameters, stations/grid points,
days, and Forecast Lead Times (FLTs). Analysis have 3 dimensions. They
are parameters, stations/grid points, and time.

## Generate Temperature Forecasts

Now that we have a general idea of the data, we can generate temperature
forecasts. The steps to generate AnEn forecasts are as follow:

-   Initialize a configuration
-   Set up options
-   Generate AnEn

First, we set up the configuration. We generate AnEn for one day using
about one year of historical data for all the grid points available. If
you don’t understand any of the options, please see the
[document](https://weiming-hu.github.io/AnalogsEnsemble/R/reference/generateConfiguration.html).

Now we can generate AnEn.

```
AnEn <- generateAnalogs(config)
```

    ## [1] "Warning: Converting data/time to numeric for search_times."
    ## [1] "Warning: Converting search_flts to a numeric vector."
    ## [1] "Warning: Converting data/time to numeric for search_observations"
    ## Convert R objects to C++ objects ...
    ## Wraping C++ object mapping ...
    ## Wraping C++ object analogs ...
    ## Done!

Then we need to bias correct AnEn because we are using AnEn on a gridded
product.

```
AnEn <- biasCorrection(AnEn, config, forecast.ID = 1, group.func = mean,
                       na.rm = T, show.progress = F)
```

`AnEn` is a list of results. Let’s check what we have in it.

```
print(AnEn)
```

    ## Class: AnEn list
    ## Member 'analogs': [test station][test time][FLT][member][type]
    ## 121 1 36 19 3 (value, search station, search observation time)
    ## Member 'analogs.cor': [test station][test time][FLT][member][type]
    ## 121 1 36 19 3 (value, search station, search observation time)
    ## Member 'mapping': [FLT][forecast time] 36 370

## Visualization

Finally, we can plot together the analysis, the forecasts, the AnEn, and
the bias corrected AnEn. Below the figures are the respective RMSE.

![Result comparison](https://github.com/Weiming-Hu/AnalogsEnsemble/blob/gh-pages/assets/posts/2018-11-04-demo-1-RAnEn-basics/results.png)

