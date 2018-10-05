# Parallel Ensemble Forecast Package

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f4cf23c626034d92a3bef0ba169a218a)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Weiming-Hu/AnalogsEnsemble&amp;utm_campaign=Badge_Grade)
[![Build Status](https://travis-ci.com/Weiming-Hu/AnalogsEnsemble.svg?token=yTGL4zEDtXKy9xWq1dsP&branch=master)](https://travis-ci.com/Weiming-Hu/AnalogsEnsemble)
[![codecov](https://codecov.io/gh/Weiming-Hu/AnalogsEnsemble/branch/master/graph/badge.svg?token=tcGGOTyHHk)](https://codecov.io/gh/Weiming-Hu/AnalogsEnsemble)
[![lifecycle](https://img.shields.io/badge/lifecycle-experimental-orange.svg)](https://www.tidyverse.org/lifecycle/#experimental)

_This document is still under development._

* [About](#about)
* [Requirement and Dependencies](#requirement-and-dependencies)
    * [On Mac](#on-mac)
    * [On Linux](#on-linux)
* [Installation](#installation)
    * [C Program and Libraries](#c-program-and-libraries)
    * [R Package](#r-package)
            * [Use Released Tarball File](#use-released-tarball-file)
            * [Compile from Source](#compile-from-source)
    * [CMake Tunable Parameters Look-up](#cmake-tunable-parameters-look-up)
* [Known Issues](#known-issues)
* [Feedbacks](#feedbacks)

## About

Parallel Ensemble Forecast package uses a numerical weather prediction algorithm, called Analog Ensemble (AnEn), to generate ensemble forecasts. Ensemble members, also referred to as analogs, are selected based on the similarity between the current multivariate forecasts and the historical forecasts. It has been successfully applied to forecasts of several weather variables, for example, short-term temperature and wind speed predictions.

This package contains several libraries and applications:

- [CAnEn](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/CAnEn): This is the main library that is implemented in C++. It provides main functionality of the AnEn method;
- [CAnalogsIO](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/CAnEnIO): This is the library for file I/O. Currently, it supports reading and writing [standard NetCDF](https://www.unidata.ucar.edu/software/netcdf/).
- [RAnalogs](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/RAnalogs): This is a R library which provides the interface for *CAnEn* functionality, in order to appeal researchers who are prefer R. This R package is called *RAnEn* which stands for *R Analogs Ensemble*.
- [Analog Generator](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/app_analogGenerator): This is an application for generating analogs. It is currently under-development.
- [Analog Selector](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/app_analogSelector): This is an application for selecting analogs based on the similarity matrices. It is currently under-development.
- [Similarity Calculator](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/app_similarityCalculator): This is an application for calculating the similarity matrices.

**This package has been developed and tested on Linux and MacOS and is not guaranteed on Windows.**

## Requirement and Dependencies

A list of requirement and dependency is provided below. Note that you don't necessarily have to install them all before installing the CAnEn library because some of them can be automatically installed during the make process, or because you may not want to install only parts of the modules.

| Dependency |                                            Description                                           |
|:----------:|:------------------------------------------------------------------------------------------------:|
|    CMake   |                                   Required for the C++ program.                                  |
|  GCC/Clang |                                   Required for the C++ program.                                  |
|  NetCDF-C  |        Optional for the C++ program. If it is not found, the project will try to build it.       |
|  Boost C++ | Optional for the C++ program. It is recommended to let the project build it for the C++ program. |
|   CppUnit  |                         Optional for the C++ program when building tests.                        |
|      R     |                                    Required for the R library.                                   |
|   OpenMP   |                                   Optional for both R and C++.                                   |

### On Mac

It is fairly straightforward to use [HomeBrew](https://brew.sh/) to manage packages. However, note that `HomeBrew` might not be working well with `Port`. If you already have one of the two, go with the desired one.

```
# Install GNU compilers to support OpenMP
# brew search gcc
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

### C Program and Libraries

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
CC=[full path to CC] CXX=[full path to CXX] cmake ..
```

Read the output messages. If you would like to make any changes and generate new make files, please delete all the files in `build/` folder first. Otherwise, you will not change anything.

After having the make files generated, please go ahead and install the program.

```
make

# Or if you are using UNIX system, use the flag -j[number of cores] to parallelize the make process
make -j4

# Build document if desired. The /html and the /man folders will be in your build directory
make document
```

After the compilation, the programs and libraries should be in the foler AnalogsEnsemble/output. 

If you want to clean up the folder, please do the following.

```
make clean
```

### R Package

##### Use Released Tarball File

Generally, the following R packages are needed:

- [BH](https://cran.r-project.org/web/packages/BH/index.html)
- [Rcpp](https://cran.r-project.org/web/packages/Rcpp/index.html)

 If you are using `Windows`, please install the following programs:

- [R for Windows](https://cran.r-project.org/bin/windows/base/)
- [Latest Rtools](https://cran.r-project.org/bin/windows/Rtools/)

Please find a tarball file under [`CAnalogs/RAnalogs/releases`](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/releases). That is the package file you need to install inside R later. Then, open an R session, and run the following command.

```
#install.packages("BH")
#install.packages("Rcpp")

# The quiet option is to reduce the amount of standard output. Switch the parameter
# if you prefer to see the full list of warnings
#
install.packages("[full path to the tarball file]", type = "source", repo = NULL, quiet = T)
```

On *Windows*, please separate folder names with `\\`.

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

The following command generates a new tarball file into the `releases` folder and overwrites the old file if file already existst.

```
make build-release
```

Clean up the files after the make process.

```
make clean-rsrc
make clean
```

### CMake Tunable Parameters Look-up

|      Parameter      |                                              Explanation                                              |       Default      |
|:-------------------:|:-----------------------------------------------------------------------------------------------------:|:------------------:|
|          CC         |                                         The C compiler to use.                                        | [System dependent] |
|         CXX         |                                        The C++ compiler to use.                                       | [System dependent] |
|    INSTALL\_RAnEn   |                                 Build and install the `RAnEn` library.                                |         OFF        |
|     BOOST\_TYPE     | `BUILD` for building `Boost` library; `SYSTEM` for using the `Boost` library installed on the system. |        BUILD       |
|  CMAKE\_BUILD\_TYPE |                          `Release` for release mode; `Debug` for debug mode.                          |       Release      |
| CMAKE\_BUILD\_TESTS |                                        Build and install tests.                                       |         OFF        |
|    BUILD\_NETCDF    |                                        Build `NetCDF` library.                                        |         OFF        |
|     BUILD\_HDF5     |                                         Build `HDF5` library.                                         |         OFF        |
|       VERBOSE       |                                            Verbose output.                                            |         OFF        |

_Table generated from [Tables Generator](https://www.tablesgenerator.com/markdown_tables)._

## Known Issues

- `RAnEn` is built with GNU compiler and CMake process. But it shows up not supporting `OpenMP`.

If you have run the `roxygen` command before the make process, the library will not be supporting `OpenMP`. Make sure that before your CMake's runtime, you have a `clean` directory under `RAnalogs/RAnEn/src`. An easy way to do this is to run `make clean-roxygen`, and repeat the build and make process.

- Linking errors when building the C++ library during the second time.

If you have successfully built the C++ library before and when you modifed any files or did a `git pull`, you found the `make` process failed. This can be caused by using the old dependencies previously built and stored in the `dependency/` directory. This typically happens when you've also changed the compiler. An easy workaround for this is to remove all the build files, include the directories `dependency\`, `output\`, and `build\`, and start from scratch with a clean directory.

- How do I make sure whether `OpenMP` is supported.

If you are using the `RAnEn` library, you can check the automatic message printed on loading the library. If it does not say "OpenMP is not supported", then it is supported.

If you are using the C++ programs, you need to first build tests by specifying `-DCMAKE_BUILD_TESTS=ON` to `cmake`. Then after the program compiles, there is a test at `AnalogsEnsemble/output/test/runAnEn`. If you see `OpenMP is supported` and the number of threads in the standard output message after running the test executable, `OpenMP` is correctly configured. You can also try to modify the system environment variable `OMP_NUM_THREADS` and run the test executable again, see how the system environment variable affects the number of threads being created.

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
# Author: Weiming Hu <weiming@psu.edu>
#         Guido Cervone <cervone@psu.edu>
#
# Contributor: 
#         Laura Clemente-Harding
#         Martina Calovi
#         
# Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
# Department of Geography and Institute for CyberScience
# The Pennsylvania State University
```
