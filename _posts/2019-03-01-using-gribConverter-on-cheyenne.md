---
layout: post
title: How to Automate Data Preprocessing for AnEn Computation on Cheyenne
tags:
  - tutorial
  - gribConverter
---

<!-- vim-markdown-toc GitLab -->

* [Introduction](#introduction)
* [Data Preparation](#data-preparation)
* [Scripts](#scripts)
* [Results](#results)
* [Appendix](#appendix)

<!-- vim-markdown-toc -->

## Introduction

This tutorial shows how to use the data preprocessing tools (`gribConverter`, `windFieldCalculator`) in the AnEn package to prepare the data to the correct form that can be directly a number of computation tools (`similarityCalculator`, `analogGenerator`, `RAnEn`) to generate analog ensembles. Addition to that, this tutorial also shows how to automate and parallelize the process.

This tutorial assumes the basic knowledge on bash script language and that `AnEn` package has already been successfully installed. More information of how to install `AnEn` on Cheyenne can be found [here](https://weiming-hu.github.io/AnalogsEnsemble/2019/02/17/build-on-cheyenne.html).

## Data Preparation

A large collection (~5.4TB) of data from [North American Mesoscale Forecast model](https://www.ncdc.noaa.gov/data-access/model-data/model-datasets/north-american-mesoscale-forecast-system-nam) have been downloaded for the time period from October, 2008, to July, 2018. The original files are `.g2.tar` files and an example for the file name is `nam_218_2008102900.g2.tar`. Files have already been arranged by `YearMonth` in each folder as follows:

```
> ls
200810  200907  201004  201101  201110  201207  201304  201401  201410  201507  201604  201701  201710  201807
200811  200908  201005  201102  201111  201208  201305  201402  201411  201508  201605  201702  201711  
200812  200909  201006  201103  201112  201209  201306  201403  201412  201509  201606  201703  201712  
200901  200910  201007  201104  201201  201210  201307  201404  201501  201510  201607  201704  201801  
200902  200911  201008  201105  201202  201211  201308  201405  201502  201511  201608  201705  201802
200903  200912  201009  201106  201203  201212  201309  201406  201503  201512  201609  201706  201803
200904  201001  201010  201107  201204  201301  201310  201407  201504  201601  201610  201707  201804
200905  201002  201011  201108  201205  201302  201311  201408  201505  201602  201611  201708  201805
200906  201003  201012  201109  201206  201303  201312  201409  201506  201603  201612  201709  201806
```

## Scripts

I have prepared two scripts. The first script is the resource PBS script. 

```
#!/bin/bash

# The name of the task
#PBS -N process_each_month

# The project account
#PBS -A ****

# The time resources requested
#PBS -l walltime=10:00:00

# The queue type
#PBS -q regular

# Combine standard output and errors
#PBS -j oe                           

# The computing resources requested
#PBS -l select=1:ncpus=1:mem=109GB:ompthreads=1

# I would like to receive an email when tasks
# (a)bort, (b)egin, and (e)nd.
#
#PBS -m abe

# And this is the email
#PBS -M *****
                                         
# Set TMPDIR as recommended
setenv TMPDIR /glade/scratch/$USER/temp
mkdir -p $TMPDIR

# Loop through the folder names
declare -a arr=("200810" "200811" "200812" "200901" "200902" "200903" "200904" "200905" "200906" "200907" "200908" "200909" "200910" "200911" "200912" "201001" "201002" "201003" "201004" "201005" "201006" "201007" "201008" "201009" "201010" "201011" "201012" "201101" "201102" "201103" "201104" "201105" "201106" "201107" "201108" "201109" "201110" "201111" "201112" "201201" "201202" "201203" "201204" "201205" "201206" "201207" "201208" "201209" "201210" "201211" "201212" "201301" "201302" "201303" "201304" "201305" "201306" "201307" "201308" "201309" "201310" "201311" "201312" "201401" "201402" "201403" "201404" "201405" "201406" "201407" "201408" "201409" "201410" "201411" "201412" "201501" "201502" "201503" "201504" "201505" "201506" "201507" "201508" "201509" "201510" "201511" "201512" "201601" "201602" "201603" "201604" "201605" "201606" "201607" "201608" "201609" "201610" "201611" "201612" "201701" "201702" "201703" "201704" "201705" "201706" "201707" "201708" "201709" "201710" "201711" "201712" "201801" "201802" "201803" "201804" "201805" "201806" "201807")

# Define the configuration file for gribConverter. This is enclosed at the end of this tutorial.
converterConfig=/glade/u/home/wuh20/scratch/data/forecasts/forecasts.cfg

# Define the output destination
destDir=/glade/u/home/wuh20/flash/forecasts_new/

# Define the lock file name
lockFile=.lock

for month in "${arr[@]}"; do
    monthDir=/glade/u/home/wuh20/scratch/data/forecasts/$month
    
    if [ -f $destDir/$month\.nc  ]; then
        echo Month $month has been processed. Skip this month.
        continue
    fi
    
    # Check whether this directory exists
    if [ ! -d $monthDir  ]; then
        echo Directory not found: $monthDir
        exit 1
    fi
    
    cd $monthDir
    
    # Lock this directory
    if [ -f $lockFile  ]; then
        echo Directory $monthDir is in process. Skip this directory.
        continue
    else
        echo Lock directory $monthDir
        touch $lockFile
    fi
    
    # Unpack tar files
    echo Extracting from tar files ...
    if [ -f log_extract  ]; then
        rm log_extract
    fi

    for tarFile in *.g2.tar; do
        tar --skip-old-files -xvf $tarFile >> log_extract
    done
    
    # Convert grb2 files
    echo Converting grb2 files ...
    /glade/u/home/wuh20/github/AnalogsEnsemble/output/bin/gribConverter -c $converterConfig --folder ./ -o $month-original.nc -v 3 > log_converter
    
    # Add wind fields
    echo Adding wind fields ...
    /glade/u/home/wuh20/github/AnalogsEnsemble/output/bin/windFieldCalculator --file-in $month-original.nc --file-type Forecasts --file-out $month\.nc -U 1000IsobaricInhPaU -V 1000IsobaricInhPaV --dir-name 1000IsobaricInhPaDir --speed-name 1000IsobaricInhPaSpeed -v 3 > log_wind
    
    # Move the data file elsewhere
    echo Moving data to $destDir
    mv $month\.nc $destDir
    
    # Cleaning
    echo Cleaning ...
    rm $month-original.nc
    rm *.grb2
    
    echo Releasing the folder lock
    rm $lockFile
   
    # Each job only process one month
    echo Finished processing month $month
    exit 0
done
```

```
#!/bin/bash

totalJobs=84
submittedJobs=0

while true; do
    number=`qstat | grep "Q regular" | wc -l`
    echo The number of queued jobs: $number
    echo The number of submitted jobs: $submittedJobs
    if (( number == 0  )); then
        echo There is no queued jobs. Submit a new one.
        qsub batch_process.pbs
        submittedJobs=$((submittedJobs + 1))
        if (( submittedJobs == totalJobs  )); then
            echo $submittedJobs jobs submitted. Done!
            exit 0
        fi
    fi
    sleep 10
done
```

## Results

## Appendix

```
# This configuration file is generated for gribConverter to convert 201807 forecast
# GRIB2 files to NetCDF files for computing analog ensembles.
#

# These two options are specified in command line arguments
#folder = /home/graduate/wuh20/geolab_storage_V3/data/NAM/NCEI/forecasts/201807/extract/
#output = 201807.nc

output-type = Forecasts
ext = .grb2
flt-interval = 3600
verbose = 3

regex-time = .*nam_218_(\d{8})_\d{4}_\d{3}\.grb2$
regex-flt = .*nam_218_\d{8}_\d{4}_(\d{3})\.grb2$

# The following lines define the parameters.
# Please refer to the post on how to get variable information
# using Eccodes tools.
#
# paramId      level        typeOfLevel        name
# 260242       2            heightAboveGround  2 metre relative humidity
pars-id = 260242
levels = 2
level-types = heightAboveGround
parameters-new-name = 2MetreRelativeHumidity

# 168          2            heightAboveGround  2 metre dewpoint temperature 
pars-id = 168
levels = 2
level-types = heightAboveGround
parameters-new-name = 2MetreDewpoint

# 167          2            heightAboveGround  2 metre temperature 
pars-id = 167
levels = 2
level-types = heightAboveGround
parameters-new-name = 2MetreTemperature

# 228139       0            depthBelowLandLayer  Soil Temperature
pars-id = 228139
levels = 0
level-types = depthBelowLandLayer
parameters-new-name = SoilTemperature

# 260509       0            surface      Albedo
pars-id = 260509
levels = 0
level-types = surface
parameters-new-name = SurfaceAlbedo

# 131          1000         isobaricInhPa  U component of wind
pars-id = 131
levels = 1000
level-types = isobaricInhPa
parameters-new-name = 1000IsobaricInhPaU

# 132          1000         isobaricInhPa  V component of wind
pars-id = 132
levels = 1000
level-types = isobaricInhPa
parameters-new-name = 1000IsobaricInhPaV

# paramId     level       name
# 130         0           Temperature 
pars-id = 130
levels = 0
level-types = surface
parameters-new-name = SurfaceTemperature

# 134         0           Surface pressure 
pars-id = 134
levels = 0
level-types = surface
parameters-new-name = SurfacePressure

# 228164      0           Total Cloud Cover 
pars-id = 228164
levels = 0
level-types = unknown
parameters-new-name = TotalCloudCover

# 228228      0           Total Precipitation 
pars-id = 228228
levels = 0
level-types = surface
parameters-new-name = TotalPrecipitation

# 260087      0           Downward short-wave radiation flux 
pars-id = 260087
levels = 0
level-types = surface
parameters-new-name = DownwardShortWaveRadiation

# 260097      0           Downward long-wave radiation flux 
pars-id = 260097
levels = 0
level-types = surface
parameters-new-name = DownwardLongWaveRadiation

# 260088      0           Upward short-wave radiation flux 
pars-id = 260088
levels = 0
level-types = surface
parameters-new-name = UpwardShortWaveRadiation

# 260098      0           Upward long-wave radiation flux 
pars-id = 260098
levels = 0
level-types = surface
parameters-new-name = UpwardLongWaveRadiation

# Difference between 2 metre above ground and surface temperature
# http://mailman.ucar.edu/pipermail/wrf-users/2012/002776.html
