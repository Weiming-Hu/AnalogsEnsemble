---
layout: page
title: Changelog
sidebar_link: true
---


# PAnEn 3.5.3

- Resolve a naming bug in `RAnEn::schaakeShuffle`

# PAnEn 3.5.2

- Improve documentation for `RAnEn::verifyBrier` and `RAnEn::verifyThreatScore`
- Improve the printing information for 'class: AnEn list'

# PAnEn 3.5.1

- Add the beta function `RAnEn::schaakeShuffle`.
- Add multiple variable support for `windFieldCalculator`.
- Add support for `Forecasts` in `RAnEn::writeNetCDF`.

# PAnEn 3.5.0

- Add functions `toDateTime`, `toForecastTime`, `val2Col`, `makeTransparent`, and `plotBackground`.
- Improve `verifyCRPS`.
- Improve `RAnEn` function documentation.
- Add identification for cycle time in C++ utilities.
- Bug fixed in checking functions in `RAnEn`.
- Add grid checks in `gribConverter` for each file.
- Make `classifier` the parameter for `extendedSearch` only.
- Improve website and tutorials/posts.

# PAnEn 3.4.2

- Add slicing tool `fileSlice`.
- Add R functions `writeNetCDF`, `readForecasts`, `readObservations`, `readAnEn`, `AnEnC2R`, `verifyThreatScore`, `verifyBrier`, and `readConfig`.
- Improve visualization. Fix the x plotting range of RAnEn figures.
- Change analogGenerator code. Similarity output is sorted now.

# PAnEn 3.4.1

- Add grid subset functionality to `gribConverter` for type `Forecasts`. This function has been tested.
- Fix `RAnEn` installation script for C/C++ compilers.
- Fix `OpenMP` shared object issue for `runtime_error`.

# PAnEn 3.4.0

- Introducing SSE functionality based on classifiers. Search space can now be computed based on a classification map that only stations that belong to the same class will be considered when computing search space.
- SSE based on classification has been ported to R interfaces.
- Minor bugs fixed.

# PAnEn 3.3.1

- Fix an indexing bug in `RAnEn` operational mode.
- Fix a parallelization bug in the internal verification function `anen.mean`.
- 'analogGenerator' accepts multiple test forecast files.
- Add partial reading feature in `fileAggregate` for `Similarity` and `Analogs`.
- Improve efficiency of verification functions. Some missing functions have been resolved.
- Add `FLT_radius` parameter to `RAnEn`

# PAnEn 3.3.0

- Add parameters to restrict the number of similarities to keep. This can be very helpful to reduce the memory required. The C++ parameter is `max-num-sims` and the R parameter is `max_num_sims`.
- Change package name from `Parallel Ensemble Forecast` to `Parallel Analog Ensemble`.
- `similarityCalculator`, `analogSelector`, `analogGenerator` now accept multiple configuration files.
- Add `along` parameter check to `AnEnIO`.
- 'analogGenerator' accepts multiple search forecast and observation files.
- Add parallelization to verification functions.

# PAnEn 3.2.6

- Add features to C++ tools that `similarityCalculator` and `analogSelector` now accept multiple file input.
- Add proper wall time profiling in C++ tools `similarityCalculator` and `analogSelector`.
- Change the profiling output messages.

# PAnEn 3.2.5

- Add function `RAnEn::plotAnalogSample` and `RAnEn::plotAnalogSelection`.
- Add the feature for `RAnEn` to preserve standard deviations.
- Add tests for R and C++ for similarity calculation and standard deviation calculation.
- Add verification functions.
- Change R interface input parameters to constant references.
- Bug fixed for the `RAnEn` configuration parameter `num_nearest` and `num_max_search_stations`.
- Bug fixed in `RAnEn::verifyCorrelation`.
- Add mapping calculation feature to analog selector.

# PAnEn 3.2.4

- Bug fixed for using weights.
- Bug fixed for using partial compare times from test and search. This was causing stack overflow.
- Change the onload message of `RAnEn`.
- Add density limit parameter for `RAnEn::biasCorrectionInsitu`.

# PAnEn 3.2.3

- Port `max_flt_nan` and `max_par_nan` parameters to R function `compute_analogs`.
- Update R and C++ document.
- Add R configuration `preserve_search_stations` to `independentSearch` as well. Now, both `independentSearch` and `extendedSearch` support this configuration.
- Add `RAnEn::alignObservations`

# PAnEn 3.2.2
- Improve backward compatibility of R function `compute_analogs`. This function now returns the analog member index in the 4D observations, rather than in the 3D observations.
- Add a cleaner version of configuration. Configuration now, by default, is the cleaner version. The old version can be accessed by using `advanced` parameter when generating a configuration.
- Remove some of the post contents to reduce redundant information. Tutorials are now only hosted on binder.
- Add MPI implementation for "MPI_UNIVERSE_SIZE" to limit the number of child spawned processes.
- Add unique identifier in `SimilarityMatrices` and `Analogs` to distinguish the reason for the NAN value.

# PAnEn 3.2.1

- Remove computing sds outside the loops when operational search is used.
- Add demo for using operational search.
- Performance improved for computing similarity.
- Add default value in `RAnEn` for `configuration$test_forecasts` and `configuration$test_times`. If they are not specified, they are copied from `configuration$search_forecasts` and `configuration$search_times` respectively.
- Add default value for `test-forecast-nc` in `analogGenerator` and `similarityCalculator`. If they are not provided, the search file will be also used as a test file.
- When MPI is used, the reading function should automatically detect whether the file can be accessed with parallel processes.

# PAnEn 3.2.0

- Synchronize versions of all components.
- Add similarity computing methods including SeparateTestSearch, LeaveOneOut, and OperationalSearch. This has brought an significant addition to similarity calculation function features.
- Add automatic removal of the overlapping test and search times. For example, if forecasts are 3 days ahead, the historical forecast for yesterday will not be compared to the forecast of today even if the yesterday forecast is included in the search times (which is considered as a mistaken input from the user).
- Add R function `generateTimeMapping`.

# PAnEn 3.1.1

## RAnEn

- Refactor Similarity Calculation core function and improve readability.
- Resolve the potential bug in the function `biasCorrectionInsitu`.
- Remove return\_index option from computeSearchStations.
- Skip processing the similarity array if it is not preserved in the result.
- Update documentation.
- Update README file.

# PAnEn 3.1.0

## CAnEnIO

- Add MPI read functions for values only.
- Fix bug for MPI functions in reading.
- Add serial program limit to AnEn library.
- Add MPI read functions for strings.
- Speed up the station matching process.

## RAnEn

- Reduce code size of the `computeSimilarity` function.
- Resolve a potential bug in `computeSimilarity` and introduce a new parameter `extend_observations` to this function. Now, `extend_observations` is supported by both `computeSimilarity` and `selectAnalogs` functions.
- Minor changes to `RAnEn::biasCorrection`.
- Introduce a new function for bias correction using a neural network.
- Change authors of the package.
- Update documentation.
- Change names of members from bias correction.
- Change import functions to suggested for `parallel`, `nnet`, and `pbapply`.

# PAnEn 3.0.0

## CAnEnIO

- Basic AnEn I/O functions.

