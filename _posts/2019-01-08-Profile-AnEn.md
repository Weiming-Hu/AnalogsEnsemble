---
layout: post
title: Tutorial on AnEn Profiling with gprof and TAU
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

Experiments are carried out on Penn State ICS cluster ACI-b.

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

## Profiling with `GProf`

### Build Programs for `GProf`

Load modules needed by AnEn programs.

```
module load gcc/5.3.1 boost/1.61.0 netcdf/4.4.1 git hdf5/1.8.18 netcdf-cxx/4.3.0
```

Build and install AnEn programs with the extra option `-pg`.

```
# Build programs
cd [Analog Ensemble Source Dir]
mkdir build && cd build

# Please note the part '/opt/aci/sw/netcdf-cxx/4.3.0_gcc-5.3.1' in CMAKE_PREFIX_PATH.
# This is the path for the NetCDF C++4 library. You can find it in the system
# environment variable CPLUS_INCLUDE_PATH
#
cmake -DCMAKE_CXX_FLAGS='-O3 -pg' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$BOOST_ROOT;$NETCDF;/opt/aci/sw/netcdf-cxx/4.3.0_gcc-5.3.1" -DCMAKE_BUILD_TESTS=ON -DBOOST_TYPE=SYSTEM -DNETCDF_CXX4_TYPE=SYSTEM -DCMAKE_INSTALL_PREFIX=release ..
make -j 8

# Test programs. Make sure all tests are shown passed.
make test

# Install programs to the release folder under the current directory
make install
```

To make an initial check that programs are correctly built. Run the program and check whether it outputs the desired file.

```
# Change the directory to the folder with programs
cd release/bin
./analogGenerator

# This file should exists.
file gmon.out
```

### Profiling AnEn

The data used here are synthetic data. They are generated using the provided R script. First, run the program normally to generate profile data.

```
# Submit an interactive job
qsub -A [Allocation Name] -I -l nodes=1:ppn=20 -l walltime=4:00:00

# Wait until the interactive session starts
module load gcc/5.3.1 boost/1.61.0 netcdf/4.4.1 git hdf5/1.8.18 netcdf-cxx/4.3.0

cd [Profile Data Dir]
rm output.nc
OMP_NUM_THREADS=20 [Analog Ensemble Source Dir]/build/release/bin/analogGenerator -c config.cfg --analog-nc output.nc
```

This should generate a `gmon.out` file. Then this file can be used by `gprof` to generate profile information.

```
gprof [Analog Ensemble Source Dir]/build/release/bin/analogGenerator gmon.out > profile.txt
```

### Visualization

To visualize the `GProf` output, we can convert the text file to a dot graph and then an image. I'm using the [gprof2dot](https://github.com/jrfonseca/gprof2dot) program to do that. I'm using the python program and I'm doing this on my local machine so that I have `sudo` permission.

```
# Install the graphviz if you do not have it
sudo apt install graphviz

virtualenv env-gprof
source env-gprof/bin/activate
pip install gprof2dot

# -w for wrapping function names
# -s for stripping detailed function information to reduce texts
#
gprof2dot -w -s profile.txt | dot -Tpng -Gdpi=500 -o profile-gprof.png
```


## Profiling with TAU

### Build Programs with TAU
First, several modules need to be loaded.

```
module load gcc/5.3.1 boost/1.61.0 netcdf/4.4.1 git hdf5/1.8.18 netcdf-cxx/4.3.0 git
```

Then, build TAU with `openmp` support. Please download the latest version of TAU and extract the file. We can use the following commands to install TAU profiler.

```
cd [TAU Source Dir]
./configure -openmp -prefix=[TAU Sourec Dir]/release
export PATH=[TAU Source Dir]/release/x86_64/bin:$PATH
make install
```

Because TAU will automatically look for some system environment variables, add the following lines to the file `~/.bashrc` (Created one if you do not have it.). The first line tell TAU to use a specific make file for `openmp` and the second line tells TAU to use compiler-based instrumentation.

```
export TAU_MAKEFILE=[TAU Source Dir]/release/x86_64/lib/Makefile.tau-openmp
export TAU_OPTIONS=-optCompInst
```

Then, we can compile AnEn programs using the TAU compiler.

```
# Build AnEn programs
cd [Analog Ensemble Source Dir]
mkdir build && cd build

# Please note the part '/opt/aci/sw/netcdf-cxx/4.3.0_gcc-5.3.1' in CMAKE_PREFIX_PATH.
# This is the path for the NetCDF C++4 library. You can find it in the system
# environment variable CPLUS_INCLUDE_PATH
#
CC=tau_cc.sh CXX=tau_cxx.sh cmake -DCMAKE_PREFIX_PATH="$BOOST_ROOT;$NETCDF;[TAU Source Dir]/release/;/opt/aci/sw/netcdf-cxx/4.3.0_gcc-5.3.1/;" -DNETCDF_CXX4_TYPE=SYSTEM -DBOOST_TYPE=SYSTEM -DCMAKE_BUILD_TESTS=ON -DCMAKE_INSTALL_PREFIX=release -DCMAKE_BUILD_TYPE=Debug ..
make -j 8

# Test the programs. Please be patient.
# Test 4 might take about 120 seconds to finish.
#
make test

# Install the program locally
make install
```

### Profiling AnEn

```
# Submit an interactive job
qsub -A [Allocation Name] -I -l nodes=1:ppn=20 -l walltime=4:00:00

# Wait until the interactive session starts and continue
module load gcc/5.3.1 boost/1.61.0 netcdf/4.4.1 git hdf5/1.8.18 netcdf-cxx/4.3.0
cd [Profile Data Dir]
rm output.nc

# TAU adds overhead to the program so we use a smaller chunk of the data sets.
OMP_NUM_THREADS=20 [Analog Ensemble Source Dir]/build/release/bin/analogGenerator -c config.cfg --analog-nc output.nc --test-start 0 0 0 0 --test-count 10 100 10 5 --search-start 0 0 0 0 --search-count 10 100 500 5 --obs-start 0 0 0 --obs-count 1 100 13140
```

### Visualization

Once the profile is generated, profile files with names like `profile.0.0.*` will be generated. We can use the following tools to visualize the results.

```
# Run this command in the folder with the profile log files.
pprof

# paraprof requires X11 window forwarding. You will need the option -Y when you log onto the cluster.
paraprof
```

The 3D visulizer might not be working properly on the ICS ACI cluster. You can [install TAU locally](http://www.paratools.com/tau). The TAU user manual is [here](https://www.cs.uoregon.edu/research/paracomp/tau/tauprofile/docs/usersguide.pdf).

## Sequel on TAU Installation

I found [TAU](https://www.cs.uoregon.edu/research/tau/home.php) profiler to be very powerful and convenient to use. It is a piece of software from the University of Oregon. However, I always have problem installing the software locally. The [video](http://www.paratools.com/tau) that I found is very helpful but there are several typos and I really prefer looking at the code to the video.

Therefore, here is the code I used to build `TAU` with OpenMP and MPI support, and the visualizer on a Ubuntu system. These codes are compiled and modified from the video.

```
# Please note that I have installed MPI in my system.

# Download all modules required and unzip them
wget http://tau.uoregon.edu/tau.tgz
tar xzf tau.tgz
cd tau-[x.x]/
wget http://tau.uoregon.edu/pdt.tgz
wget http://tau.uoregon.edu/ext.tgz
tar xzf pdt.tgz
tar xzf ext.tgz

# Configure pdtoolkit
# Look out for messages that ask you to add a directory to your PATH
#
cd pdtoolkit-[x.x]/
./configure

# Use -j on Linux to parallelize the build process
make 
make install
cd ..

# Configure TAU
# Look out for messages that ask you to add a directory to your PATH
#
./configure -c++=mpicxx -cc=mpicc -mpi -openmp -bfd=download -unwind=download -pdt=[path to pdt]

# Use -j on Linux to parallelize the build process
make
make install

# Add the Makefile file path as a system variable
export TAU_MAKEFILE=[your path to tau dir]/tau/x86_64/lib/Makefile.tau-mpi-pdt-openmp

# Also add the TAU Option.
# I used -optCompInst to let the compiler insert instrumentation automatically.
# I used -optNoRevert to force the compiler causes hard-failure when there is a TAU error.
#
export TAU_OPTIONS="-optCompInst -optNoRevert"
```

For `TAU_OPTIONS`, you can find the references [here](https://www.alcf.anl.gov/user-guides/tuning-and-analysis-utilities-tau). At this point, I have successfully built `TAU` with the visualizer `paraprof`. 
