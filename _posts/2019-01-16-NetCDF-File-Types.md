---
layout: post
title: NetCDF File Types and Variables for Analog Ensemble Applications
tags:
  - document
---


<!-- vim-markdown-toc GitLab -->

* [Introduction](#introduction)
* [File Types](#file-types)
    * [Forecasts](#forecasts)
    * [Observations](#observations)
    * [Analogs](#analogs)
    * [Similarity](#similarity)
    * [StandardDeviation](#standarddeviation)
    * [Matrix](#matrix)
* [References](#references)

<!-- vim-markdown-toc -->

## Introduction

Under the [apps](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/apps) directory, there are several C++ programs that implements different phases of generating analog ensembles, including calculating standard deviations, calculating similarity metrics, and selecting analog forecasts, and some other programs for data pre-processing. Currently, all input and output files are in NetCDF format. This articles documents variables and dimensions expected in each file type based on the file type, for example, Forecasts, Observations, Similarity, and so on.

## File Types

The defined [file types](https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_an_en_i_o.html#addbfb455f641a394c14907163874d8fe) include:

- Forecasts
- Observations
- Analogs
- Similarity
- StandardDeviation 
- Matrix

Each file type is associated with a list of expected dimensions and a list of expected variables. Those variables and dimensions are required to ensure the correctness and performance of C++ program. Some variables can also be very helpful during visualization.

### [Forecasts](https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_forecasts.html)

An example `Forecasts` file includes the following content:

```
9 variables (excluding dimension variables):
   char ParameterNames[num_chars,num_parameters]   (Contiguous storage)  
   double ParameterWeights[num_parameters]   (Contiguous storage)  
   char ParameterCirculars[num_chars,num_parameters]   (Contiguous storage)  
   char StationNames[num_chars,num_stations]   (Contiguous storage)  
   double Xs[num_stations]   (Contiguous storage)  
   double Ys[num_stations]   (Contiguous storage)  
   double Times[num_times]   (Contiguous storage)  
   double FLTs[num_flts]   (Contiguous storage)  
   double Data[num_parameters,num_stations,num_times,num_flts]   (Contiguous storage)  

5 dimensions:
   num_parameters  Size:17
   num_chars  Size:50
   num_stations  Size:262792
   num_times  Size:31
   num_flts  Size:53
```

- **ParameterNames** are the names of each parameters in the forecasts.
- **ParameterWeights** are the corresponding weight for each parameter in the forecasts to be used when computing forecast similarity.
- **ParameterCirculars** are the names of the circular parameters.
- **StationNames** are the names of the forecast stations or grid points.
- **Xs** are the x coordinates of the forecast stations or grid points.
- **Ys** are the y coordinates of the forecast stations or grid points.
- **Times** are the time representation of forecasts. It is the number of seconds since the origin, [1970-01-01 00:00:00 UTC](https://weiming-hu.github.io/AnalogsEnsemble/CXX/classanen_time_1_1_times.html#a7e08602fb0628df1c5f1cccbb98baeb1) by default.
- **FLTs** are the time representation of forecast lead times. It is the number of seconds since the initialization of the forecast model.
- **Data** is a 4-dimensional array that stores the actual forecast values.

### [Observations](https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_observations.html)

An example `Observations` file looks pretty much similar `Forecasts`, except that the variable **Data** is a 3-dimensional array without forecast lead times.

```
8 variables (excluding dimension variables):
   char ParameterNames[num_chars,num_parameters]   (Contiguous storage)  
   double ParameterWeights[num_parameters]   (Contiguous storage)  
   char ParameterCirculars[num_chars,num_parameters]   (Contiguous storage)  
   char StationNames[num_chars,num_stations]   (Contiguous storage)  
   double Xs[num_stations]   (Contiguous storage)  
   double Ys[num_stations]   (Contiguous storage)  
   double Times[num_times]   (Contiguous storage)  
   double Data[num_parameters,num_stations,num_times]   (Contiguous storage)  

4 dimensions:
   num_parameters  Size:15
   num_chars  Size:50
   num_stations  Size:262792
   num_times  Size:496
```

### [Analogs](https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_analogs.html)

An example `Analogs` file includes the following content:

```
10 variables (excluding dimension variables):
   double Analogs[num_test_stations,num_test_times,num_flts,num_members,num_cols]   (Contiguous storage)  
   double FLTs[num_flts]   (Contiguous storage)  
   char StationNames[num_chars,num_stations]   (Contiguous storage)  
   double Xs[num_stations]   (Contiguous storage)  
   double Ys[num_stations]   (Contiguous storage)  
   double Times[num_times]   (Contiguous storage)  
   char MemberStationNames[num_chars,member_num_stations]   (Contiguous storage)  
   double MemberXs[member_num_stations]   (Contiguous storage)  
   double MemberYs[member_num_stations]   (Contiguous storage)  
   double MemberTimes[member_num_times]   (Contiguous storage)  

10 dimensions:
   num_test_stations  Size:20
   num_test_times  Size:1
   num_flts  Size:8
   num_members  Size:5
   num_cols  Size:3
   num_stations  Size:20
   num_chars  Size:50
   num_times  Size:1
   member_num_stations  Size:20
   member_num_times  Size:80
```

- **Analogs** is a 5-dimensional array that stores analog forecasts. More information about analogs can be found at [here](https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_analogs.html).
- **FLTs** is the time representation of the analog forecasts. It is the number of seconds since the initialization of the forecast model.
- **StationNames** are the names of stations for analog forecasts.
- **Xs** are the x coordinates of stations for analog forecasts.
- **Ys** are the y coordinates of stations for analog forecasts.
- **Times** is the time representation of the analog forecasts. It is the number of seconds since the origin, [1970-01-01 00:00:00 UTC](https://weiming-hu.github.io/AnalogsEnsemble/CXX/classanen_time_1_1_times.html#a7e08602fb0628df1c5f1cccbb98baeb1) by default.
- **MemberStationNames** are the names of stations for analog members. This can be used together with the search station index in the fifth dimension to get the exact details of search station used.
- **MemberXs** are the x coordinates of stations for analog members. This can be used together with the search station index in the fifth dimension to get the exact details of search station used.
- **MemberYs** are the y coordinates of stations for analog members. This can be used together with the search station index in the fifth dimension to get the exact details of search station used.
- **MemberTimes** is the time representation of the search times. This can be used together with the search time index in the fifth dimension to know what historical time this member belongs to.

### [Similarity](https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_similarity_matrices.html)

An example `Similarity` file includes the following content:

```
11 variables (excluding dimension variables):
   double SimilarityMatrices[num_cols,num_entries,num_flts,num_test_times,num_test_stations]   (Contiguous storage)  
   char ParameterNames[num_chars,num_parameters]   (Contiguous storage)  
   double ParameterWeights[num_parameters]   (Contiguous storage)  
   char ParameterCirculars[num_chars,num_parameters]   (Contiguous storage)  
   char StationNames[num_chars,num_stations]   (Contiguous storage)  
   double Xs[num_stations]   (Contiguous storage)  
   double Ys[num_stations]   (Contiguous storage)  
   double Times[num_times]   (Contiguous storage)  
   double FLTs[num_flts]   (Contiguous storage)  
   double SearchTimes[num_search_times]   (Contiguous storage)  
   char SearchStationNames[num_chars,num_search_stations]   (Contiguous storage)  
   double SearchXs[num_search_stations]   (Contiguous storage)  
   double SearchYs[num_search_stations]   (Contiguous storage)  

10 dimensions:
   num_test_stations  Size:20
   num_test_times  Size:1
   num_flts  Size:8
   num_entries  Size:9
   num_cols  Size:3
   num_parameters  Size:5
   num_chars  Size:50
   num_stations  Size:20
   num_times  Size:1
   num_search_times  Size:9
   num_search_stations  Size:20
```

- **SimilarityMatrices** is a 5-dimensional array that stores similarity metric values.
- **ParameterNames** are names of parameters used to calculate the similarity.
- **ParameterWeights** are weights of parameters used to calculate the similarity.
- **ParameterCirculars** are names of circular parameters.
- **StationNames** are names of stations or grid points for which similaity is generated.
- **Xs** are x coordinates of stations or grid points for which similaity is generated.
- **Ys** are y coordinates of stations or grid points for which similaity is generated.
- **Times** is the time representation of the similarity. It is the number of seconds since the origin, [1970-01-01 00:00:00 UTC](https://weiming-hu.github.io/AnalogsEnsemble/CXX/classanen_time_1_1_times.html#a7e08602fb0628df1c5f1cccbb98baeb1) by default.
- **FLTs** is the time representation of the similarity. It is the number of seconds since the initialization of the forecast model.
- **SearchTimes** are times for the complete search period. This can be used together with the search time index in the fifth dimension to know what historical forecast this similarity is generated from.
- **SearchStationNames** are stations names for the complete search data. This can be used together with the search station index in the fifth dimension to know what station/grid point is used to generate similarity.
- **SearchXs** are x coordinates for the complete search stations. This can be used together with the search station index in the fifth dimension to know what station/grid point is used to generate similarity.
- **SearchYs** are y coordinates for the complete search stations. This can be used together with the search station index in the fifth dimension to know what station/grid point is used to generate similarity.

### [StandardDeviation](https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_standard_deviation.html)

An example `StandardDeviation` file includes the following content:

```
8 variables (excluding dimension variables):
   double StandardDeviation[num_parameters,num_stations,num_flts]   (Contiguous storage)  
   char ParameterNames[num_chars,num_parameters]   (Contiguous storage)  
   double ParameterWeights[num_parameters]   (Contiguous storage)  
   char ParameterCirculars[num_chars,num_parameters]   (Contiguous storage)  
   char StationNames[num_chars,num_stations]   (Contiguous storage)  
   double Xs[num_stations]   (Contiguous storage)  
   double Ys[num_stations]   (Contiguous storage)  
   double FLTs[num_flts]   (Contiguous storage)  

4 dimensions:
   num_parameters  Size:4
   num_stations  Size:4
   num_flts  Size:4
   num_chars  Size:50
```

- **StandardDeviation** is a 3-dimensional array that stores standard deviation values.
- **ParameterNames** are the names of parameters.
- **ParameterWeights** are the weights of parameters.
- **ParameterCirculars** are the names of circular parameters.
- **StationNames** are the names of stations or grid points.
- **Xs** are the x coordinates of stations or grid points.
- **Ys** are the y coordinates of stations or grid points.
- **FLTs** are the forecast lead times.

### Matrix

File type `Matrix` is designed for time mapping matrix between forecast times/forecast lead times and observation times. It is usually in text file format.

## References

All the example output is generated using R package [ncdf4](https://cran.r-project.org/web/packages/ncdf4/index.html).
