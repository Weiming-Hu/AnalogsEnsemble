---
layout: post
title: Building AnEn on NCAR Cheyenne
tags:
  - tutorial
---

<!-- vim-markdown-toc GitLab -->

* [Introduction](#introduction)
* [Building AnEn](#building-anen)

<!-- vim-markdown-toc -->

Introduction
------------

This short tutorial walks you through the steps of building the AnEn C++ program on [NCAR Cheyenne Supercomputers](https://www2.cisl.ucar.edu/resources/computational-systems/cheyenne/cheyenne).

Building AnEn 
------------

Several things to be noted before we carry on:

- Most of the dependencies are already available on Cheyenne, so I'm going to load them directly. `Boost`, however, is not available, so I tell `cmake` to build it for me.
- I will be installing `PAnEn` into a user space folder after the successful building. You can change the argument `CMAKE_INSTALL_PREFIX`.
- Notice the argument `CMAKE_INSTALL_RPATH`. This is needed because the modules are not in system path. When we install programs, `cmake` by default removes build-time run path, so we need to specify the run-time path for install and where the executable should be looking for libraries.

```
# Download the source files
wget https://github.com/Weiming-Hu/AnalogsEnsemble/archive/master.zip

# Unzip the tarball
unzip master.zip

# Go to the source folder
cd AnalogEnsemble-master

# Clean modules
module purge

# Load required modules
module load gnu/9.1.0 netcdf/4.7.3 ncarenv/1.3 cmake/3.16.4 eccodes/2.12.5

# Carry an out-of-tree build
mkdir build
cd build

# Generate build system
cmake -DCMAKE_INSTALL_PREFIX=../../release -DBUILD_BOOST=ON -DCMAKE_PREFIX_PATH="$NCAR_ROOT_ECCODES;$NETCDF" -DCMAKE_INSTALL_RPATH="$NCAR_ROOT_ECCODES/lib;$NETCDF/lib" ..

# Build
make -j 16

# Test
make test

# Instal
make install

# Show help message
cd ../../release/bin
./anen
```

If you encountered any problems, please open a ticket [here](https://github.com/Weiming-Hu/AnalogsEnsemble/issues).
