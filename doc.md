---
layout: default
---


# [PAnEn: Parallel Analog Ensemble](https://weiming-hu.github.io/AnalogsEnsemble/)

[![DOI](https://zenodo.org/badge/130093968.svg)](https://zenodo.org/badge/latestdoi/130093968)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/e4b8f97d5f3a41029741579f648c8e38)](https://www.codacy.com/gh/Weiming-Hu/AnalogsEnsemble/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Weiming-Hu/AnalogsEnsemble&amp;utm_campaign=Badge_Grade)
[![Build C++](https://github.com/Weiming-Hu/AnalogsEnsemble/workflows/Build%20C++/badge.svg)](https://github.com/Weiming-Hu/AnalogsEnsemble/actions?query=workflow%3A%22Build+C%2B%2B%22)
[![Build R](https://github.com/Weiming-Hu/AnalogsEnsemble/actions/workflows/BuildR.yml/badge.svg)](https://github.com/Weiming-Hu/AnalogsEnsemble/actions/workflows/BuildR.yml)
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
    * [CMake Parameters](#cmake-parameters)
    * [High-Performance Computing and Supercomputers](#high-performance-computing-and-supercomputers)
    * [MPI and OpenMP](#mpi-and-openmp)
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

```r
install.packages("https://github.com/Weiming-Hu/AnalogsEnsemble/raw/master/RAnalogs/releases/RAnEn_latest.tar.gz", repos = NULL)
```

That's it. You are good to go. Please refer to [tutorials](#tutorials) or the [R documentation](https://weiming-hu.github.io/AnalogsEnsemble/R/) to learn more about using `RAnEn`. You might also want to install [RAnEnExtra](https://github.com/Weiming-Hu/RAnEnExtra) package with functions for visualization and verification. After `RAnEn` installation, you can simply run `devtools::install_github("Weiming-Hu/RAnEnExtra")`.

**Mac users**: if the package shows that `OpenMP` is not supported. You can do one of the followings: 

1. Avoid using Clang compilers and convert to GNU compilers. To change the compilers used by R, create a file `~/.R/Makevars` if you do not have it already and add the following content to it. Of course, change the compilers to what you have. If you do not have any alternative compilers other than Clang, [HomeBrew](https://brew.sh/) is your friend.

```bash
CC=gcc-8
CXX=g++-8
CXX1X=g++-8
CXX14=g++-8
```

2. You can also follow the instructions [here](https://github.com/Rdatatable/data.table/wiki/Installation#openmp-enabled-compiler-for-mac) provided by `data.table`. They provide similar solutions but stick with Clang compilers.

After the installation, you can always revert back to your original setup and `RAnEn` will stay supported by `OpenMP`.

### CAnEn

To install the C++ libraries, please check the following dependencies.

- _Required_ [CMake](https://cmake.org/) is the required build system generator.
- _Required_ [NetCDF](https://www.unidata.ucar.edu/downloads/netcdf/) provides the file I/O with NetCDF files.
- _Required_ [Eccodes](https://confluence.ecmwf.int/display/ECC/ecCodes+installation) provides the file I/O with Grib2 files.
- _Optional_ [Boost](https://www.boost.org/) provides high-performance data structures.  `Boost` is a very large library. If you don't want to install the entire package, `PAnEn` is able to build the required ones automatically.
- _Optional_ `CppUnit` provides test frameworks. If `CppUnit` is found in the system, test programs will be compiled.

To set up the dependency, it is recommended to use [conda](https://docs.conda.io/en/latest/miniconda.html). I chose `minicoda` instead of `anaconda` simply beacause `miniconda` is the light-weight version. If you already have `anaconda`, you are fine as well.

The following code sets up the environment from stratch:

```bash
# Python version is required because of boost compatibility issues
conda create -n venv_anen python==3.8 -y

# Keep your environment activate during the entire installation process, including CAnEn
conda activate venv_anen

# Required dependency
conda install -c anaconda cmake boost -y
conda install -c conda-forge netcdf-cxx4 eccodes doxygen  -y

# Optional dependency: LibTorch
# If you need libTorch, please go ahead to https://pytorch.org/get-started/locally/ and select
# Stable -> [Your OS] -> LibTorch -> C++/Java -> [Compute Platform] -> cxx11 ABI version
# 
# Please see https://github.com/Weiming-Hu/AnalogsEnsemble/issues/86#issuecomment-1047442579 for instructions
# on how to inlcude libTorch during the cmake process.

# Optional dependency: MPI
conda install -c conda-forge openmpi -y
```

After the dependencies are installed, let's build `CAnEn`:

```bash
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

### CMake Parameters

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
|    ENABLE\_OPENMP    |                                       Enable multi-threading with OpenMP                                                                     |         ON         |
|     ENABLE\_AI       |                               Enable PyTorch integration and the power of AI.                                                                |         OFF         |

You can change the default of the parameters, for example, `cmake -DCMAKE_INSTALL_PREFIX=~/AnalogEnsemble ..`. Don't forget the extra letter `D` when specifying argument names.

### High-Performance Computing and Supercomputers

[Here](https://github.com/Weiming-Hu/AnalogsEnsemble/issues/86) is a list of instructions to build and install `AnEn` on supercomputers. 


### MPI and OpenMP 

```
TL;DR

Launching an MPI-OpenMP hybrid program can be tricky.

If the performance with MPI is acceptable,
disable OpenMP (`cmake -DENABLE_OPENMP=OFF ..`).

If the hybrid solution is desired,
make sure you have the proper setup.
```

When `ENABLE_MPI` is turned on, MPI programs will be built. These MPI programs are hybrid programs (unless you set `-DENABLE_OPENMP=OFF` for `cmake`) that use both MPI and OpenMP. Please check with your individual supercomputer platform to find out **what the proper configuration for launching an MPI + OpenMP hybrid program is**. Users are responsible not to launch too many process and threads at the same time which would overtask the machine and might lead to hanging problems (as what I have seen on [XSEDE Stampede2](https://portal.tacc.utexas.edu/user-guides/stampede2)).

On [NCAR Cheyenne](https://www2.cisl.ucar.edu/resources/computational-systems/cheyenne), the proper way to launch a hybrid program can be found [here](https://www2.cisl.ucar.edu/resources/computational-systems/cheyenne/running-jobs/pbs-pro-job-script-examples). If you use `mpirun`, instead of `mpiexec_mpt`, you will loose the multi-threading performance improvement.

To dive deeper into the hybrid parallelization design, MPI is used for computationally expensive portions of the code, e.g. file I/O and analog generation while OpenMP is used by the master process during bottleneck portion of the code, e.g. data reshaping and information queries.

When analogs with a long search and test periods are desired, MPI is used to distribute forecast files across processes. Each process reads a subset of the forecast files. This solves the problem where serial I/O can be very slow.

When a large number of stations/grids present, MPI is used to distribute analog generation for different stations across processes. Each process takes charge of generating analogs for a subset of stations.

Sitting between the file I/O and the analog generation is the bottleneck which is hard to parallelize with MPI, e.g. reshaping the data and querying test/search times. Therefore, they are parallelized with OpenMP on master process only.

So if the platform support heterogeneous task layout, users can theoretically allocate one core per worker process and more cores for the master process to facilitate its multi-threading scope. But again, only do this when you find the bottleneck is taking much longer time than file I/O and analog generation. Use `--profile` to have profiling information in standard message output.

## Tutorials

Tutorials can be accessed on [binder](https://mybinder.org/v2/gh/Weiming-Hu/AnalogsEnsemble/master?urlpath=rstudio) or be found in [this directory](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/RAnalogs/examples)

Here are also some tips and caveats in [this ticket](https://github.com/Weiming-Hu/AnalogsEnsemble/issues/81).

## References

- [Delle Monache, Luca, et al. "Probabilistic weather prediction with an analog ensemble." Monthly Weather Review 141.10 (2013): 3498-3516.](https://journals.ametsoc.org/doi/full/10.1175/MWR-D-12-00281.1)
- [Clemente-Harding, L. A Beginners Introduction to the Analog Ensemble Technique](https://ral.ucar.edu/sites/default/files/public/images/events/WISE_documentation_20170725_Final.pdf)
- [Cervone, Guido, et al. "Short-term photovoltaic power forecasting using Artificial Neural Networks and an Analog Ensemble." Renewable energy 108 (2017): 274-286.](https://www.sciencedirect.com/science/article/pii/S0960148117301386)
- [Junk, Constantin, et al. "Predictor-weighting strategies for probabilistic wind power forecasting with an analog ensemble." Meteorol. Z 24.4 (2015): 361-379.](https://www.researchgate.net/profile/Constantin_Junk/publication/274951815_Predictor-weighting_strategies_for_probabilistic_wind_power_forecasting_with_an_analog_ensemble/links/552cd5710cf29b22c9c47260.pdf)
- [Balasubramanian, Vivek, et al. "Harnessing the power of many: Extensible toolkit for scalable ensemble applications." 2018 IEEE International Parallel and Distributed Processing Symposium (IPDPS). IEEE, 2018.](https://ieeexplore.ieee.org/abstract/document/8425207)
- [Hu, Weiming, and Guido Cervone. "Dynamically Optimized Unstructured Grid (DOUG) for Analog Ensemble of numerical weather predictions using evolutionary algorithms." Computers & Geosciences 133 (2019): 104299.](https://www.sciencedirect.com/science/article/pii/S0098300418306678)

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
