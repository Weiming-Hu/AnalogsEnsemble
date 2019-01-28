# [Parallel Ensemble Forecast Package](https://weiming-hu.github.io/AnalogsEnsemble/)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f4cf23c626034d92a3bef0ba169a218a)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Weiming-Hu/AnalogsEnsemble&amp;utm_campaign=Badge_Grade)
[![Build Status](https://travis-ci.com/Weiming-Hu/AnalogsEnsemble.svg?token=yTGL4zEDtXKy9xWq1dsP&branch=master)](https://travis-ci.com/Weiming-Hu/AnalogsEnsemble)
[![codecov](https://codecov.io/gh/Weiming-Hu/AnalogsEnsemble/branch/master/graph/badge.svg?token=tcGGOTyHHk)](https://codecov.io/gh/Weiming-Hu/AnalogsEnsemble)
[![lifecycle](https://img.shields.io/badge/lifecycle-experimental-orange.svg)](https://www.tidyverse.org/lifecycle/#experimental)
[![Binder](https://mybinder.org/badge.svg)](https://mybinder.org/v2/gh/Weiming-Hu/AnalogsEnsemble/master)

<!-- vim-markdown-toc GitLab -->

* [About](#about)
* [Requirement and Dependencies](#requirement-and-dependencies)
    * [On Mac](#on-mac)
    * [On Linux](#on-linux)
* [Installation](#installation)
    * [C Programs and Libraries](#c-programs-and-libraries)
    * [R Package](#r-package)
            * [Use Released Tarball File](#use-released-tarball-file)
            * [Compile from Source](#compile-from-source)
    * [CMake Tunable Parameters Look-up](#cmake-tunable-parameters-look-up)
* [Known Issues](#known-issues)
* [Feedbacks](#feedbacks)

<!-- vim-markdown-toc -->

## About

Parallel Ensemble Forecast package uses a numerical weather prediction algorithm, called Analog Ensemble (AnEn), to generate ensemble forecasts. Ensemble members, also referred to as analogs, are selected based on the similarity between the current multivariate forecasts and the historical forecasts. It has been successfully applied to forecasts of several weather variables, for example, short-term temperature and wind speed predictions.

A more detailed description of the Analog Ensemble technique can be found in [this post](https://weiming-hu.github.io/AnalogsEnsemble/2018/12/14/AnEn-explained.html).

This package contains several libraries and applications:

- __AnEn__: This is the main library that is implemented in C++. It provides main functionality of the AnEn method;
- __AnEnIO__: This is the library for file I/O. Currently, it supports reading and writing [standard NetCDF](https://www.unidata.ucar.edu/software/netcdf/).
- __RAnEn__: This is an R package which provides C++ library *AnEn* functionality to facilitate researchers who prefer R. *RAnEn* stands for *R Analogs Ensemble*.
- __Apps__: You can find multiple applications included in `apps/` folder designed for *AnEn* computation and data management.

**These packages are developed and tested on Linux and MacOS and not guaranteed on Windows.**

## Requirement and Dependencies

A list of requirement and dependency is provided below. Note that you don't necessarily have to install them all before installing the CAnEn library because some of them can be automatically installed during the make process, or because you may not want to install only parts of the modules.

| Dependency |                                            Description                                           |
|:----------:|:------------------------------------------------------------------------------------------------:|
|    CMake   |                                   Required for the C++ program.                                  |
|  GCC/Clang |                                   Required for the C++ program.                                  |
|  NetCDF-C  |        Optional for the C++ program. If it is not found, the project will try to build it.       |
|  Boost C++ | Optional for the C++ program. It is recommended to let the project build it for the C++ program. |
|   CppUnit  |                         Required for the C++ program when building tests.                        |
|      R     |                                    Required for the R library.                                   |
|   OpenMP   |                                   Optional for both R and C++.                                   |

### On Mac

It is fairly straightforward to use [HomeBrew](https://brew.sh/) to manage packages. However, note that `HomeBrew` might not be working well with `Port`. If you already have one of the two, go with the desired one.

```
# Install GNU compilers to support OpenMP
#
# brew search gcc
# 
# Select version from the output of the above command, I choose gcc-7 below.
#
brwe install gcc@7

# Install CMake
# brew search cmake
brew install cmake

# Install NetCDF
brew install netcdf
```

### On Linux

If `apt-get` is available, please see the following commands.

```
# Install NetCDF
sudo apt-get install libnetcdf-dev netcdf-bin

```

## Installation

### C Programs and Libraries

The installation process can be divided into two steps:

- generate make files
- build programs and libraries

First, let's create a `build/` folder where it stores our automatically generated files. This is also called an out-of-source build.

```
cd AnalogsEnsemble
mkdir build
cd build
```

Then, generate the make files.

```
cmake ..

# If you would like to change the default compiler, specify the compilers like this
#
# CC=[full path to CC] CXX=[full path to CXX] cmake ..
```

Read the output messages. If you would like to make any changes and generate new make files, please delete all the files in `build/` folder first. Otherwise, you will not change anything.

Some of the components are by default turned off. Please refer to [CMake Tunable Parameters Look-up](#cmake-tunable-parameters-look-up) for instructions for how to turn them on.

After having the make files generated, please go ahead and compile the programs and the libraries.

```
make

# Or if you are using UNIX system, use the flag -j[number of cores] to parallelize the make process
make -j4

# Build document if desired. The /html and the /man folders will be in your build directory
make document
```

After the compilation, the programs and libraries should be in the folder `AnalogsEnsemble/output`. For example, to print out the help messages for the program `analogCalculator`, open up a terminal and `cd` into the binary folder `[Where you download the repository]/AnalogsEnsemble/output/bin/` and run the following command.

```
./analogGenerator 
# Analog Ensemble program --- Analog Generator
# Available options:
#  -h [ --help ]             Print help information for options.
#  -c [ --config ] arg       Set the configuration file path. Command line 
#                            options overwrite options in configuration file. 
# ...
```

If you want to clean up the folder, please do the following.

```
make clean
```

### R Package

_Please use the installation guide for updating the package as well. The process is the same._

##### Use Released Tarball File

Generally, the following R packages are needed:

- [BH](https://cran.r-project.org/web/packages/BH/index.html)
- [Rcpp](https://cran.r-project.org/web/packages/Rcpp/index.html)

Please run the following commands in R and make sure they succeed. 

```
install.packages('Rcpp')
install.packages('BH')
```

If you are using `Windows`, please install the following programs:

- [R for Windows](https://cran.r-project.org/bin/windows/base/)
- [Latest Rtools](https://cran.r-project.org/bin/windows/Rtools/)

The following command installs the latest version of `RAnEn`. Please run the following command in R.

```
install.packages("https://github.com/Weiming-Hu/AnalogsEnsemble/raw/master/RAnalogs/releases/RAnEn_latest.tar.gz", repos = NULL)
```

If you want to install a specific version of `RAnEn`, you can go to the [release folder](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/RAnalogs/releases), copy the full name of the tarball file, replace the following part `[tarball name]` (including the square bracket) with it, and run the following command in R.

```
install.packages("https://github.com/Weiming-Hu/AnalogsEnsemble/raw/master/RAnalogs/releases/[tarball name]", repos = NULL)
```

Or, you can download the latest tarball file from the [release folder](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/RAnalogs/releases). That is the package file you need to install inside R later. If you have already downloaded this repository, you can also find the release package tarball file under the folder `[Where you downloaded this repository]/AnalogsEnsemble/RAnalogs/releases`. Then, open an R session, and run the following command.

```
# The quiet option is to reduce the amount of standard output. Switch the parameter
# if you prefer to see the full list of warnings
#
install.packages("[full path to the tarball file]", type = "source", repos = NULL, quiet = T)

# On Windows, please separate folder names with `//`.
# For example, the full path to the tarball file on a Windows should look similar to this
# C://Users//yourName//Downloads//RAnEn_3.0.6.tar.gz
```

If you want to use a different compiler, an easy workaround is to create a `Makevars` file under `~/.R`, with the following content.

```
CXX1X=[C++11 compiler]

# required on Mac OS
CXX11=[C++11 compiler]
```

After this, you should see the same compiler in the standard output.

##### Compile from Source

First, let's create a `build/` folder where it stores our automatically generated files.

```
cd AnalogsEnsemble
mkdir build
cd build
```

Then, let's generate the make files.

```
cmake -DINSTALL_RAnEn=ON -G "Unix Makefiles" ..
```

Read the output messages. If you would like to make any changes and generate new make files, please delete all the files in `build/` folder first. Otherwise, you will not change anything because of the existed files.

After the make files have been generated, go ahead and install the R package.

```
make
```

The following command installs the R package.

```
make install
```

Clean up the files after the make process.

```
make clean
make clean-all
```

### CMake Tunable Parameters Look-up

|      Parameter       |                                              Explanation                                                |       Default      |
|:--------------------:|:-------------------------------------------------------------------------------------------------------:|:------------------:|
|          CC          |                                         The C compiler to use.                                          | [System dependent] |
|          CXX         |                                        The C++ compiler to use.                                         | [System dependent] |
|     INSTALL\_RAnEn   |                                 Build and install the `RAnEn` library.                                  |         OFF        |
|      BOOST\_TYPE     | `BUILD` for building `Boost` library; `SYSTEM` for using the library on the system.                     |        BUILD       |
|  NETCDF\_CXX4\_TYPE  | `BUILD` for building `Netcdf C++4` library; `SYSTEM` for using the library on the system.               |        BUILD       |
|     CPPUNIT\_TYPE    | `BUILD` for building `CppUnit` library; `SYSTEM` for using the `CppUnit` library from the system.       |        BUILD       |
|   CMAKE\_BUILD\_TYPE |                          `Release` for release mode; `Debug` for debug mode.                            |       Release      |
|  CMAKE\_BUILD\_TESTS |                                             Build tests.                                                |         OFF        |
|     BUILD\_NETCDF    |                           Build `NetCDF` library regardless of its existence.                           |         OFF        |
|      BUILD\_HDF5     |                            Build `HDF5` library regardless of its existence.                            |         OFF        |
|        VERBOSE       |                          Print detailed messages during the compiling process.                          |         OFF        |
|    CODE\_PROFILING   |                                     Print time profiling information.                                   |         OFF        |
|     ENABLE\_MPI      |                         Build the MPI version of the CAnEnIO library for parallel I/O.                  |         OFF        |
| BUILD\_GRIBCONVERTER | Build the GRIB Converter utility. [Eccodes](https://confluence.ecmwf.int/display/ECC) library required. |         ON         |

## Known Issues

Please see known issues in [this post](https://weiming-hu.github.io/AnalogsEnsemble/2018/10/17/known-issues.html). If you could not find solutions to your issue, you can consider open a ticket or comment below the post. Thank you.

## Feedbacks

We appreciate collaborations and feedbacks from users. Please contact maintainer [Weiming Hu](http://weiming.ddns.net) through [weiming@psu.edu](weiming@psu.edu), or create tickets if you have any problems.

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
#
# Contributors: 
#     Laura Clemente-Harding <lec170@psu.edu>
#     Martina Calovi <mcalovi@psu.edu>
#     Luca Delle Monache
#         
# Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
# Department of Geography and Institute for CyberScience
# The Pennsylvania State University
```
