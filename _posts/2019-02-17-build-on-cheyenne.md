---
layout: post
title: Building AnEn on NCAR Cheyenne
tags:
  - tutorial
---

<!-- vim-markdown-toc GitLab -->

* [Introduction](#introduction)
* [Building AnEn without MPI](#building-anen-without-mpi)
* [Building AnEn with MPI](#building-anen-with-mpi)
* [Building `gribConverter`](#building-gribconverter)


<!-- vim-markdown-toc -->

Introduction
------------

This short tutorial walks you through the steps of building the AnEn C++ program on [NCAR Cheyenne Supercomputers](https://www2.cisl.ucar.edu/resources/computational-systems/cheyenne/cheyenne) with and without MPI support.


Building AnEn without MPI
------------

```
# Download the source files from Github
git clone https://github.com/Weiming-Hu/AnalogsEnsemble.git

# Go to the source folder
cd AnalogEnsemble

# Clean modules and load required modules
module purge && module load git gnu/8.1.0 netcdf/4.6.1

# Carry an out-of-tree build
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=$NETCDF -DCMAKE_INSTALL_PREFIX=../release ..
make -j 72 install

# Test
make test

# Test show analogGenerator
cd ../release/bin
./analogGenerator
```

Building AnEn with MPI
------------

```
# Download the source files from Github
git clone https://github.com/Weiming-Hu/AnalogsEnsemble.git

# Go to the source folder
cd AnalogEnsemble

# Clean modules and load required modules
module purge && module load git gnu/8.1.0 mpt/2.18 netcdf-mpi/4.6.1

# Carry an out-of-tree build
mkdir build && cd build
CC=mpicc CXX=mpicxx cmake -DCMAKE_PREFIX_PATH=$NETCDF -DCMAKE_INSTALL_PREFIX=../release -DENABLE_MPI=ON ..
make -j 72 install

# Test 
export MPI_UNIVERSE_SIZE=10
mpiexec_mpt -np 1 ../output/test/runAnEnIO
```

Building `gribConverter`
------------

The package provides a utility ,`gribConverter`, for converting grib2 files. It relies on [Eccodes](https://confluence.ecmwf.int/display/ECC/ecCodes+installation) which can be automatically built.

If you wish to build `gribConverter`, you only need to add `-DBUILD_GRIBCONVERTER=ON` in your cmake arguments and the utility will be built. The rest of the process stays the same.
