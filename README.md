# Parallel Ensemble Forecast Package

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f4cf23c626034d92a3bef0ba169a218a)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Weiming-Hu/AnalogsEnsemble&amp;utm_campaign=Badge_Grade)
[![Build Status](https://travis-ci.com/Weiming-Hu/AnalogsEnsemble.svg?token=yTGL4zEDtXKy9xWq1dsP&branch=master)](https://travis-ci.com/Weiming-Hu/AnalogsEnsemble)

_This document is still under development._

* [About](#about)
* [Requirement and Dependencies](#requirement-and-dependencies)
* [Installation](#installation)
	* [C Program and Libraries](#c-program-and-libraries)
	* [R Package](#r-package)
			* [Use Released Tarball File](#use-released-tarball-file)
			* [Compile from Source](#compile-from-source)
	* [CMake Tunable Parameters Look-up](#cmake-tunable-parameters-look-up)
* [Feedbacks](#feedbacks)

## About

Parallel Ensemble Forecast package uses a numerical weather prediction algorithm, called Analog Ensemble (AnEn), to generate ensemble forecasts. Ensemble members, also referred to as analogs, are selected based on the similarity between the current multivariate forecasts and the historical forecasts. It has been successfully applied to forecasts of several weather variables, for example, short-term temperature and wind speed predictions.

This package contains several libraries and applications:

- [CAnEn](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/CAnEn): This is the main library that is implemented in C++. It provides main functionality of the AnEn method;
- [CAnalogsIO](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/CAnalogsIO): This is the library for file I/O. Currently, it supports reading and writing [standard NetCDF](https://www.unidata.ucar.edu/software/netcdf/).
- [RAnalogs](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/RAnalogs): This is a R library which provides the interface for *CAnEn* functionality, in order to appeal researchers who are prefer R. This R package is called *RAnEn* which stands for *R Analogs Ensemble*.
- [Analogs Generator](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/app_analogsGenerator): This is an application for generating analogs. It is currently under-development.
- Similarity Calculator: This is an application for computing the similarity metrics. It is currently under development.
- Analogs Evaluator: This is an application for evaluating the similarity metrics and selecting the analogs. It is currently under development.

**This package has been developed and tested on Linux and MacOS and is not guaranteed on Windows.**

## Requirement and Dependencies

A list of requirement and dependency is provided below. Note that you don't necessarily have to install them all before installing the CAnEn library because some of them can be automatically installed during the make process, or because you may not want to install only parts of the modules.

- CMake
- GCC/Clang
- NetCDF-C, which depends on HDF5, cul, and zlib
- Boost
- OpenMP
- R
- CppUnit

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

// Or if you are using UNIX system
// use the flag -j[number of cores] to parallelize the make process
make -j4
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

|      Parameter      |                        Usage                        |                                                                                                              Explanation                                                                                                              |
|:-------------------:|:---------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
|          CC         |          `CC=/usr/local/bin/gcc-7 cmake ..`         |                                                                                                                                                                                            Specify C compiler. Usually used with CXX. |
|         CXX         |         `CXX=/usr/local/bin/g++-7 cmake ..`         |                                                                                                                                                                                           Specify C++ compiler. Usually used with CC. |
|    INSTALL\_RAnEn   |            `cmake -DINSTALL_RAnEn=OFF ..`           |                                                                                                                                                                            **Default**. Specify installing the C++ executable prgram. |
|                     |            `cmake -DINSTALL_RAnEn=ON ..`            |                                                                                                                                                                                                 Specify installing the RAnEn package. |
|     BOOST\_TYPE     |            `cmake -DBOOST_TYPE=BUILD ..`            |                                                                                                                                                                                        **Default**. Specify building Boost libraries. |
|                     |            `cmake -DBOOST_TYPE=SYSTEM ..`           |                                                                                                                                                                            **Not recommended**. Specify using system Boost libraries. |
|  CMAKE\_BUILD\_TYPE |        `cmake -DCMAKE_BUILD_TYPE=Release ..`        |                                                                                                                                                               **Default**. Specify Release which will generate less warning messages. |
|                     |         `cmake -DCMAKE_BUILD_TYPE=Debug ..`         |                                                                                                                                                                              Specify Debug which will generate more warning messages. |
| CMAKE\_BUILD\_TESTS |          `cmake -DCMAKE_BUILD_TESTS=ON ..`          |                                                                                                                                                                                                               Specify to build tests. |
|                     |          `cmake -DCMAKE_BUILD_TESTS=OFF ..`         |                                                                                                                                                                                              **Default**. Specify not to build tests. |
|    BUILD\_NETCDF    |             `cmake -DBUILD_NETCDF=ON ..`            |                                                                                                                                                                                                                 Build NetCDF library. |
|    BUILD\_HDF5      |             `cmake -DBUILD_HDF5=ON ..`            |                                                                                                                                                                                                                 Build HDF5 library. |
|     ENABLE\_MPI     |             `cmake -DENABLE_MPI=OFF ..`             |                                                                                                                                           **Default**. Specify whether to build MPI support. **This is currently under development.** |
|       VERBOSE       |               `cmake -DVERBOSE=OFF ..`              |                                                                                                                                                                                                             **Default**. Less output. |
|                     |               `cmake -DVERBOSE=ON ..`               |                                                                                                                                                                                                                      Detailed output. |

_Table generated from [Tables Generator](https://www.tablesgenerator.com/markdown_tables)._

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
