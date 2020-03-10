# [PAnEn: Parallel Analog Ensemble](https://weiming-hu.github.io/AnalogsEnsemble/)

[![DOI](https://zenodo.org/badge/130093968.svg)](https://zenodo.org/badge/latestdoi/130093968)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f4cf23c626034d92a3bef0ba169a218a)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Weiming-Hu/AnalogsEnsemble&amp;utm_campaign=Badge_Grade)
[![Build Status](https://travis-ci.com/Weiming-Hu/AnalogsEnsemble.svg?token=yTGL4zEDtXKy9xWq1dsP&branch=master)](https://travis-ci.com/Weiming-Hu/AnalogsEnsemble)
[![codecov](https://codecov.io/gh/Weiming-Hu/AnalogsEnsemble/branch/master/graph/badge.svg?token=tcGGOTyHHk)](https://codecov.io/gh/Weiming-Hu/AnalogsEnsemble)
[![lifecycle](https://img.shields.io/badge/lifecycle-experimental-orange.svg)](https://www.tidyverse.org/lifecycle/#experimental)
[![Binder](https://mybinder.org/badge.svg)](https://mybinder.org/v2/gh/Weiming-Hu/AnalogsEnsemble/master?urlpath=rstudio)
[![dockeri.co](https://dockeri.co/image/weiminghu123/panen)](https://hub.docker.com/r/weiminghu123/panen)

<!-- vim-markdown-toc GitLab -->

* [Overview](#overview)
* [Citation](#citation)
* [Installation](#installation)
    * [RAnEn](#ranen)
    * [CAnEn](#canen)
* [Tutorials](#tutorials)
* [References](#references)
* [Feedbacks](#feedbacks)

<!-- vim-markdown-toc -->

## Overview

Parallel Analog Ensemble (PAnEn) generates accurate forecast ensembles relying on a single deterministic model simulation and the historical observations. The technique was introduced by Luca Delle Monache et al. in the paper [Probabilistic Weather Prediction with an Analog Ensemble](https://journals.ametsoc.org/doi/full/10.1175/MWR-D-12-00281.1). Developed and maintained by [GEOlab](http://geolab.psu.edu) at Penn State, PAnEn aims to provide an efficient implementation for this technique and user-friendly interfaces in R and C++ for researchers who want to use this technique in their own research.

The easiest way to use this package is to install the R package, 'RAnEn'. C++ libraries are also available but they are designed for intermediate users with requirement for performance. For installation guidance, please refer to the [installation section](#installation). 

## Citation

To cite this package, you have several options:

- Using `LaTex`: Please use [this file](https://github.com/Weiming-Hu/AnalogsEnsemble/blob/master/RAnalogs/RAnEn/inst/CITATION) for citation.
- Using `R`: Simply type `citation('RAnEn')` and the citation message will be printed.
- Using plain text: Please use the following citation format:

```
Weiming Hu, Guido Cervone, Laura Clemente-Harding, and Martina Calovi. (2019). Parallel Analog Ensemble. Zenodo. http://doi.org/10.5281/zenodo.3384321
```

## Installation

`RAnEn` is very easy to install if you are already using [R](https://www.r-project.org/). This is the recommended way to start.

### RAnEn

_The command is the same for `RAnEn` installation and update._

To install `RAnEn`, please install the following packages first:

- [BH](https://cran.r-project.org/web/packages/BH/index.html): `install.packages('BH')`
- [Rcpp](https://cran.r-project.org/web/packages/Rcpp/index.html): `install.packages('Rcpp')`
- If you are using `Windows`, please also install the latest version of [Rtools](https://cran.r-project.org/bin/windows/Rtools/).

The following R command install the latest `RAnEn`.

```
install.packages("https://github.com/Weiming-Hu/AnalogsEnsemble/raw/master/RAnalogs/releases/RAnEn_latest.tar.gz", repos = NULL)
```

That's it. You are good to go. Please refer to [tutorials](#tutorials) or the [R documentation](https://weiming-hu.github.io/AnalogsEnsemble/R/) to learn more about using `RAnEn`.

### CAnEn

To install the C++ libraries, please check the following dependencies:

- _Required_ [CMake](https://cmake.org/) is the required build system generator.
- _Required_ [NetCDF](https://www.unidata.ucar.edu/downloads/netcdf/) provides the file I/O with NetCDF files.
- _Required_ [Eccodes](https://confluence.ecmwf.int/display/ECC/ecCodes+installation) provides the file I/O with Grib2 files.
- _Optional_ [Boost](https://www.boost.org/) provides high-performance data structures.  `Boost` is a very large library. If you don't want to install the entire package, `PAnEn` is able to build the required ones automatically.
- _Optional_ `CppUnit` provides test frameworks. If `CppUnit` is found in the system, test programs will be compiled.

Please use the following scripts to install the libraries:

```
# Download the source files (~10 Mb)
wget https://github.com/Weiming-Hu/AnalogsEnsemble/archive/master.zip

# Unzip
unzip master.zip

# Create a separate folder to store all intermediate files during the installation process
cd AnalogsEnsemble-master/
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=~/AnalogEnsemble ..

# Compile
make -j 4

# Install
make install
```

Below is a list of parameters you can change and customize.

|      Parameter       |                                               Explanation                                                                                    |       Default      |
|:--------------------:|:--------------------------------------------------------------------------------------------------------------------------------------------:|:------------------:|
|  CMAKE_C_COMPILER    |                                         The C compiler to use.                                                                               | [System dependent] |
|  CMAKE_CXX_COMPILER  |                                        The C++ compiler to use.                                                                              | [System dependent] |
|CMAKE\_INSTALL\_PREFIX|                                             The installation directory.                                                                      | [System dependent] |
|  CMAKE\_PREFIX\_PATH | Which folder(s) should cmake search for packages besides the default. Paths are surrounded by double quotes and separated with semicolons.   |       [Empty]      |
| CMAKE\_INSTALL\_RPATH|                                         The run-time library path. Paths are surrounded by double quotes and separated with semicolons.      |       [Empty]      |
|   CMAKE\_BUILD\_TYPE |                          `Release` for release mode; `Debug` for debug mode.                                                                 |       Release      |
|     INSTALL\_RAnEn   |                                 Build and install the `RAnEn` library.                                                                       |         OFF        |
|     BUILD\_BOOST     |                                 Build `Boost` regardless of whether it exists in the system.                                                 |         OFF        |
|     BOOST\_URL       |                        The URL for downloading Boost. This is only used when `BUILD_BOOST` is `ON`.                                          | [From SourceForge] |
|     ENABLE\_MPI      |                        Build the MPI supported libraries and executables. This requires the MPI dependency.                                  |         OFF        |

You can change the default of the parameters, for example, `cmake -DCMAKE_INSTALL_PREFIX=~/AnalogEnsemble ..`. Don't forget the extra letter `D` when specifying argument names.

## Tutorials

Tutorials can be accessed on [binder](https://mybinder.org/v2/gh/Weiming-Hu/AnalogsEnsemble/master?urlpath=rstudio) or be found in [this directory](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/RAnalogs/examples)

Here are also some tips and caveats in [this ticket](https://github.com/Weiming-Hu/AnalogsEnsemble/issues/81).

## References

- [Delle Monache, Luca, et al. "Probabilistic weather prediction with an analog ensemble." Monthly Weather Review 141.10 (2013): 3498-3516.](https://journals.ametsoc.org/doi/full/10.1175/MWR-D-12-00281.1)
- [Clemente-Harding, L. A Beginners Introduction to the Analog Ensemble Technique](https://ral.ucar.edu/sites/default/files/public/images/events/WISE_documentation_20170725_Final.pdf)
- [Cervone, Guido, et al. "Short-term photovoltaic power forecasting using Artificial Neural Networks and an Analog Ensemble." Renewable energy 108 (2017): 274-286.](https://www.sciencedirect.com/science/article/pii/S0960148117301386)
- [Junk, Constantin, et al. "Predictor-weighting strategies for probabilistic wind power forecasting with an analog ensemble." Meteorol. Z 24.4 (2015): 361-379.](https://www.researchgate.net/profile/Constantin_Junk/publication/274951815_Predictor-weighting_strategies_for_probabilistic_wind_power_forecasting_with_an_analog_ensemble/links/552cd5710cf29b22c9c47260.pdf)
- [Balasubramanian, Vivek, et al. "Harnessing the power of many: Extensible toolkit for scalable ensemble applications." 2018 IEEE International Parallel and Distributed Processing Symposium (IPDPS). IEEE, 2018.](https://ieeexplore.ieee.org/abstract/document/8425207)

## Feedbacks

We appreciate collaborations and feedbacks from users. Please contact the maintainer [Weiming Hu](https://weiming-hu.github.io) through [weiming@psu.edu](weiming@psu.edu) or submit tickets if you have any problems.

Thank you!

```
# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Authors: 
#     Weiming Hu <weiming@psu.edu>
#     Guido Cervone <cervone@psu.edu>
#     Laura Clemente-Harding <lec170@psu.edu>
#     Martina Calovi <mcalovi@psu.edu>
#
# Contributors: 
#     Luca Delle Monache
#         
# Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
# Department of Geography and Institute for CyberScience
# The Pennsylvania State University
```
