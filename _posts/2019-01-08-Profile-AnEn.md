---
layout: post
title: AnEn Profiling with gprof and TAU
tags:
  - tutorial
---

<!-- vim-markdown-toc GFM -->

* [Introduction](#introduction)
* [Result Preview](#result-preview)
* [Preparation and Clarification](#preparation-and-clarification)
* [Profiling with `GProf`](#profiling-with-gprof)
    * [Build Programs for `GProf`](#build-programs-for-gprof)
    * [Profiling AnEn](#profiling-anen)
    * [Visualization](#visualization)
* [Profiling with TAU](#profiling-with-tau)
    * [Build Programs with TAU](#build-programs-with-tau)
    * [Profiling AnEn](#profiling-anen-1)
    * [Visualization](#visualization-1)
* [Sequel on TAU Installation](#sequel-on-tau-installation)

<!-- vim-markdown-toc -->

## Introduction

This file documents the process of profiling analysis of the weather forecast technique [`Analog Ensemble`](https://weiming-hu.github.io/AnalogsEnsemble/).

## Result Preview

These figures are generated using TAU profiler and the visualization tools `paraprof`.

![time-breakdown](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2019-01-08-Profile-AnEn/tau-breakdown-by-thread.png)
![time-3D](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2019-01-08-Profile-AnEn/tau-3D.png)
![time-threads](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2019-01-08-Profile-AnEn/tau-threads.png)

The following figure is generated from `gprof`.

![time-dot-graph](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2019-01-08-Profile-AnEn/gprof.png)

## Preparation and Clarification

Please note a couple of placeholders in this tutorial. It is recommended to use the absolute full path to replace them.

- [Allocation Name] is the project name you are attached to. It shows up every time when you log onto ICS.
- [Analog Ensemble Source Dir] is the root directory of Analog Ensemble programs. You can download it from [Github](https://github.com/Weiming-Hu/AnalogsEnsemble).
- [TAU Source Dir] is the folder all TAU source files are extracted to. You can download TAU [here](https://www.cs.uoregon.edu/research/tau/downloads.php);
- [Profile Data Dir] is the folder with profile data and a configuration file. Please generate the profile data using the R script [generateAnEnInput.R](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2019-01-08-Profile-AnEn/generateAnEnInput.R) by running `Rscript generateAnEnInput.R` in a console. The R package `ncdf4` is required. The configuration file is [config.cfg](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2019-01-08-Profile-AnEn/config.cfg).

## Profiling with TAU

### Build with `TAU`

Similar to `gprof`, we need to build the program with `tau` compilers. Please install `tau` first. Here, I assume that `tau` is already available. Wondering how to install `TAU`, please jump to the last section.

```
# Build AnEn programs
cd [Analog Ensemble Source Dir]
mkdir build && cd build

# Generate the make system. We are installing to a specific location to avoid any program clashing
CC=taucc CXX=taucxx cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../release_tau ..

# Sometimes, TAU might not be able to find some packages. So you might need to add -DCMAKE_PREFIX_PATH to guide tau compilers
CC=taucc CXX=taucxx cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../release_tau -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu .. 

# Build
make -j 2

# Test
make test

# Install
make install
```

### Profiling

To collect profiler data, run the program normally. It is necessary to run the program with the exact command for `gprof`.

```
cd [Profile Data Dir]
OMP_NUM_THREADS=1 [Analog Ensemble Source Dir]/release_tau/bin/anen_grib -c config.cfg
```

### Visualization

Profile files have names like `profile.0.0.*`. We can use the following tools to visualize the results.

```
# For text visualization
pprof

# For graphic visualization
paraprof
```

## Profiling with `gprof`

_Please note that `gprof` might have the highest sampling error among the three solutions here._

### Build with `gprof`

To profile the program with `gprof`, we only need to build the program with the extra flag `-pg`.

```
# Go to our root directory and carry an out-of-tree build
cd [Analog Ensemble Source Dir]
mkdir build && cd build

# Generate the make system. We are installing to a specific location to avoid any program clashing
cmake -DCMAKE_CXX_FLAGS='-pg' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../release_gprof ..

# Build
make -j 2

# Test
make test

# Install
make install
```

Let's check the program is built successfully.

```
# Change the directory to the installation folder
cd ../release/bin
./anen_grib

# The following file should be automatically generated.
file gmon.out
```

### Profiling

Run the program normally.

```
cd [Profile Data Dir]
OMP_NUM_THREADS=1 [Analog Ensemble Source Dir]/release_gprof/bin/anen_grib -c config.cfg
```

This should generate a `gmon.out` file.

### Visualization

To visualize the `gprof` output, we can convert the text file to a dot graph and then an image. I'm using the [gprof2dot](https://github.com/jrfonseca/gprof2dot) program which is written in python.

```
# Install the graphviz if you do not have it
sudo apt install graphviz

virtualenv env -p python3
source env/bin/activate
pip install gprof2dot

# -w for wrapping function names
# -s for stripping detailed function information to reduce texts
#
gprof [Analog Ensemble Source Dir]/build/release/bin/anen_grib gmon.out | gprof2dot -w -s | dot -Tpng -Gdpi=500 -o profile-gprof.png
```

## Profiling with `valgrind`

`valgrind` is very accurate because it runs your program in a virtual environment. *But it does introduces a lot of overhead (10x ~ 80x slower)*.

Check if you have already installed the profiler tools. To install them, you can use `sudo apt install kcachegrind valgrind`.

### Build

No extra configurations are needed. Just build the program as you normally would.

```
# Go to our root directory and carry an out-of-tree build
cd [Analog Ensemble Source Dir]
mkdir build && cd build

# Generate the make system. We are installing to a specific location to avoid any program clashing
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../release_valgrind ..

# Build
make -j 2

# Test
make test

# Install
make install
```

### Profiling

Run the executable with `valgrind`.

```
cd [Profile Data Dir]
export OMP_NUM_THREADS=1
time valgrind --tool=callgrind [Analog Ensemble Source Dir]/release_valgrind/bin/anen_grib -c config.cfg
```

### Visualization

Some profile data files with names like `callgrind.out.*` should have been generated. Use `kcachegrind` to visualize them. Choose the latest one if you have multiple of them. Usually this is because you have run the command multiple times.

```
kcachegrind [callgrind.out.* profile data file]
```

## Sequel on TAU Installation

I found [TAU](https://www.cs.uoregon.edu/research/tau/home.php) profiler to be very powerful and convenient to use. It is a piece of software from the University of Oregon. The [video](http://www.paratools.com/tau) walks you through the installation and I followed it. There might be typos so be careful when reading and watching.

For `TAU_OPTIONS`, you can find the references [here](https://www.alcf.anl.gov/user-guides/tuning-and-analysis-utilities-tau). At this point, I have successfully built `TAU` with the visualizer `paraprof`. 
