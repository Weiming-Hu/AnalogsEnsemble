# Parallel Ensemble Forecast Package

==This document is still under development.==

* [About](#about)
* [Dependencies](#dependencies)
* [Installation](#installation)
    * [C Program and Libraries](#c-program-and-libraries)
    * [R Package](#r-package)
            * [Use Pre-built Version](#use-pre-built-version)
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

## Dependencies

A list of dependency is provided below. Note that you don't necessarily have to install them all before installing the CAnEn library because some of them can be automatically installed during the make process, or because you may not want to install some of the modules.

- CMake
- netcdf-c, which depends on HDF5, cul, and zlib
- Boost
- OpenMP
- R

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

##### Use Pre-built Version

Generally, the following R packages are needed:

- [BH](https://cran.r-project.org/web/packages/BH/index.html)
- [Rcpp](https://cran.r-project.org/web/packages/Rcpp/index.html)

 If you are using `Windows`, please install the following programs:

- [R for Windows](https://cran.r-project.org/bin/windows/base/)
- [Latest Rtools](https://cran.r-project.org/bin/windows/Rtools/)

Please find a tarball file under `CAnalogs/RAnalogs/precompiled`. That is the package file you need to install inside R later. Then, open an R session, and run the following command.

```
#install.packages("BH")
#install.packages("Rcpp")

install.packages("[full path to the tarball file]", type = "source", repo = NULL)
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

If you have multiple R executables, please specify which R should be used.

```
cmake -DINSTALL_RAnEn=ON -DR_COMMAND=[R executable] -G "Unix Makefiles" ..
```

Read the output messages. If you would like to make any changes and generate new make files, please delete all the files in `build/` folder first. Otherwise, you will not change anything because of the existed files.

After the make files have been generated, go ahead and install the R package.

```
make
```

*Please don't use parallelizing flags `-j` here. It will mess up the order of execution and you will fail. It won't take long.*

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
|      R\_COMMAND     |       `cmake -DR_COMMAND=/usr/local/bin/r ..`       |                                                                                                                                                                                                             Specify the R executable. |
|   CONFIGURE\_ONLY   | `cmake -DINSTALL_RAnEn=ON -DCONFIGURE_ONLY=TRUE ..` | When installing the `RAnEn` package, you specify this parameter to only configure the installation. Then, you need to manually install it by hand using `R CMD BUILD` and `R CMD INSTALL`. This is for developing/debugging purposes. |
|  CMAKE\_BUILD\_TYPE |        `cmake -DCMAKE_BUILD_TYPE=Release ..`        |                                                                                                                                                               **Default**. Specify Release which will generate less warning messages. |
|                     |         `cmake -DCMAKE_BUILD_TYPE=Debug ..`         |                                                                                                                                                                              Specify Debug which will generate more warning messages. |
| CMAKE\_BUILD\_TESTS |          `cmake -DCMAKE_BUILD_TESTS=ON ..`          |                                                                                                                                                                                                               Specify to build tests. |
|                     |          `cmake -DCMAKE_BUILD_TESTS=OFF ..`         |                                                                                                                                                                                              **Default**. Specify not to build tests. |
|    BUILD\_NETCDF    |             `cmake -DBUILD_NETCDF=ON ..`            |                                                                                                                                                                                                                 Build NetCDF library. |
|     ENABLE\_MPI     |             `cmake -DENABLE_MPI=OFF ..`             |                                                                                                                                           **Default**. Specify whether to build MPI support. **This is currently under development.** |
|       VERBOSE       |               `cmake -DVERBOSE=OFF ..`              |                                                                                                                                                                                                             **Default**. Less output. |
|                     |               `cmake -DVERBOSE=ON ..`               |                                                                                                                                                                                                                      Detailed output. |

**Table generated from [Tables Generator](https://www.tablesgenerator.com/markdown_tables).

## Feedbacks

We appreciate collaborations and feedbacks from users. Please contact the maintainer [weiming@psu.edu](weiming@psu.edu), or create tickets if you have any problems.

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
