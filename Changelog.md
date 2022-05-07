---
layout: page
title: Changelog
sidebar_link: true
---


# PAnEn 4.4.5

- From this version, only keeping notes for major and minor version. Notes for patches are dismissed.
- Introduced spatial analogs with `PyTorch` and deep learning
- Added a new class `Grid` and the Python API `AnEnGrid`
- Added a new command line tool, `anen_transform`
- Added a new option `--no-norm` for AI embeddings
- Moved `Txt` class from `AnEnIO` to the lower level class `Grid`
- Migrated from C++11 to C++14 for `RAnEn` to avoid compilation error from `BH`

# PAnEn 4.3.1

- `anen_netcdf` can now accept reading from different subsets of observation and forecast stations.
- Fixed the inconsistency when writing AnEn results generated from `AnEnSSEMS`
- Added functionality for `anen_grib` and `grib_convert` so that input files can now be specified within a text file
- Added new argument `--exclude-closest-location` for SSE
- Added station subset by indices for `anen_netcdf`

# PAnEn 4.3.0

- Added `AnEnSSEMS` for matching stations. This addition has also been ported to `RAnEn`.

# PAnEn 4.2.7

- Added test time specification to C++ utility tools
- Added functionality to `CAnEnMPI` to deal with excessive processes

# PAnEn 4.2.6

- Added multi-tokens to wind speed name conversion
- Fixed a bug when reading doubles with no decimal from an NetCDF file into R. The small error will be removed when reading.

# PAnEn 4.2.5

- Added `RAnEn::readNc2`, `RAnEn::readNcAttrs`, `RAnEn::mergeList`
- Added `recursive` to `RAnEn::print.list`
- Added functions to unwrap observations when saving test observations (C++)
- Fixed a bug for multi-variate analogs when NAN indices are present

# PAnEn 4.2.4

- Moved 'RAnEn::schaakeShuffle' to `RAnEnExtra::schaakeShuffle`
- Improve efficiency of AI inference

# PAnEn 4.2.3

- Added reading root attributes to `RAnEn::readNc`
- Added `RAnEn::formatForecasts` and `RAnEn::sortStations`

# PAnEn 4.2.2

- Added debug printing when parameters are missing
- Added functionality to read `nc_STRING`
- Added embedding support for 2-dimensional embedding (parameters, lead times)
- Added default embedding type
- Added complete messages
- Fixed a bug when reading configuration arguments with multiple tokens

# PAnEn 4.2.1

- Separated AI models into an embedding model and a similarity model.
- Added OpenMP parallelization to embeddings
- `RAnEn::readConfig` now reads any `*.cfg` file into a list.

# PAnEn 4.2.0

- Added AI functionality to C++ utilities
- Add copy operators to classes

# PAnEn 4.1.3

- 'RAnEn::alignObservations' has been changed to be silent by default.
- 'RAnEn::schaakeShuffle' can find proper index for shuffling and added parameters.

# PAnEn 4.1.2

- Minor fix. When similarity value is NAN, do not copy any results to output.

# PAnEn 4.1.1

- Added `RAnEn::writeNcVar`
- Fixed partial reading check for station start and count
- Improve column-wise check

# PAnEn 4.1.0

- Added `anen_netcdf`
- Fixed missing weight problem in `anen_grib`

# PAnEn 4.0.10

- Fixed missing variable in `RAnEn::writeNetCDF`

# PAnEn 4.0.9

- Avoid printing too much information for length-of-1 list in `RAnEn::AnEn`.

# PAnEn 4.0.8

- Added thread control

# PAnEn 4.0.7

- Removed profiling current memory
- Added sorting capability to `RAnEn::formatObservations`
- Added argument to read a subset of variables to `RAnEn::readNc`
- Bug fixed in `RAnEn::formatObservations` when X and Y column names are used
- Bug fixed in `RAnEn::writeNetCDF` when `StationNames` is empty

# PAnEn 4.0.6

- Bug fixed. Analog values should not be saved when the similarity value is NAN. All other information (time and station index) will still be saved

# PAnEn 4.0.5

- Added `AnEnMPI`
- Improved `Profiler` and added more profiling information
- Progress information will be displayed every 1%
- Improved documentation

# PAnEn 4.0.4

- Changed `MPI_Init` to `MPI_Init_thread` for OpenMP programs
- Add progress information to AnEn computation
- Improved the distribution of files across nodes

# PAnEn 4.0.3

- Added the regular expressions for forecast and analysis filenames separately
- Changed from `boost::regex` to `boost::xpressive`
- Added MPI support
- Fixed a bug when reading lat/lon from grib files
- Improved the file I/O functions in `RAnEn`
- Improved the printing functions for `Forecasts` and `Observations`

# PAnEn 4.0.2

- Added test and search period checks to avoid calculating analogs over empty time periods
- Changed the tutorial on generating operational analogs
- Improve the performance of `AnEnReadGrib` by avoiding reading entire files

# PAnEn 4.0.1

- Added `RAnEn::generateSearchStations`
- Updated document links

# PAnEn 4.0.0 Pandora

- Changed Underlying data structure from boost to pointer for better performance
- Introduced functionalities like checks of searching into future
- Reduced memory consumption when using the R API
- Changed several configuration names in R
- Some functions are deprecated
- R API changed

# PAnEn 3.7.1

- Introduced auto generation of HPC work flows using `RAnEn::generateHPCWorkflow`
- Fixed a bug during subseting observation times in `RAnEn::plotAnalogTimeSeries`
- Added `operational` in `RAnEn::generateHPCWorkflow`
- `search_times_compare` automatically changed when `operational` was used
- Changed `RAnEn::writeConfiguration` from `ifelse` to using the function `if`

# PAnEn 3.7.0

- Negative values in indices were removed by default. Added a parameter `debug` for backward compatibility.
- Added function `RAnEn::formatObservations`
- Updated the docker image
- Change `require` to `requireNamespace` to avoid function name conflicts
- Added `RAnEn::subsetStations`
- Improve `RAnEn` name space management
- Added `RAnEn::plotAnalogTimeSeries`
- Changed default values for `Configuration` from `NA` to `NULL` when it is necessary

# PAnEn 3.6.5

- Added reading forecast circular parameters in `RAnEn::readForecasts
- Added station subset function `RAnEn::subsetCoordinates`
- Minor document fix
- Added Github repository link on R documentation website
- Set up binder with RStudio and removed python notebooks


# PAnEn 3.6.4

- Fixed environment issue for `RAnEn::verify`
- Bug fixed in `RAnEn::plotAnEnSelection`
- Updated the building process of `gribConverter`.
- Changed the spread skill verification function to slicing by FLTs.

# PAnEn 3.6.3

- Fixed a bug in checking the configuration object in R
- Added instructions for package citation

# PAnEn 3.6.2

- Changed the implementation of `RAnEn::verifySpreadSkill`
- Changed legal information
- Added checks and default values in `RAnEn::generateSearchStation`
- Resolved issues in tests
- Added a convenient function `RAnEn::getAssociatedForecasts`
- Used `magrittr` pipes to improve code readability
- Added the printing method for R class 'Configuration'

# PAnEn 3.6.1

- Added `RAnEn::generatePersistent`
- Added function names when `Done` is printed

# PAnEn 3.6.0

- Verification just became simpler. Added a caller function `RAnEn::verify` and helper function `RAnEn::showVerificationMethods`
- Resolved a variable missing bug in `RAnEn::writeNetCDF`

# PAnEn 3.5.4

- Changed warnings of `CAnEnIO` from `cout` to `cerr`
- Reduced copies in `RAnEn::AnEnC2R`
- Added garbage collection mechanism to R file I/O functions

# PAnEn 3.5.3

- Resolved a naming bug in `RAnEn::schaakeShuffle`
- Resolved a path bug in `RAnEn::readAnEn`
- Added `RAnEn::writeConfiguration`
- Added `ParameterCirculars` in function `RAnEn::writeNetCDF`
- Changed C++ test verbose levels

# PAnEn 3.5.2

- Improved documentation for `RAnEn::verifyBrier` and `RAnEn::verifyThreatScore`
- Improved the printing information for 'class: AnEn list'

# PAnEn 3.5.1

- Added the beta function `RAnEn::schaakeShuffle`.
- Added multiple variable support for `windFieldCalculator`.
- Added support for `Forecasts` in `RAnEn::writeNetCDF`.

# PAnEn 3.5.0

- Added functions `toDateTime`, `toForecastTime`, `val2Col`, `makeTransparent`, and `plotBackground`.
- Improved `verifyCRPS`.
- Improved `RAnEn` function documentation.
- Added identification for cycle time in C++ utilities.
- Bug fixed in checking functions in `RAnEn`.
- Added grid checks in `gribConverter` for each file.
- Made `classifier` the parameter for `extendedSearch` only.
- Improved website and tutorials/posts.

# PAnEn 3.4.2

- Added slicing tool `fileSlice`.
- Added R functions `writeNetCDF`, `readForecasts`, `readObservations`, `readAnEn`, `AnEnC2R`, `verifyThreatScore`, `verifyBrier`, and `readConfig`.
- Improved visualization. Fix the x plotting range of `RAnEn` figures.
- Changed `analogGenerator` code. Similarity output is sorted now.

# PAnEn 3.4.1

- Added grid subset functionality to `gribConverter` for type `Forecasts`. This function has been tested.
- Fixed `RAnEn` installation script for C/C++ compilers.
- Fixed `OpenMP` shared object issue for `runtime_error`.

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

