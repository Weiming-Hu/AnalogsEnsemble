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

This tutorial shows how to use the data preprocessing tools (`gribConverter`, `windFieldCalculator`) in the AnEn package to reformat the data to the correct form that can be directly used by a number of computation tools (`similarityCalculator`, `analogGenerator`, `RAnEn`) to generate analog ensembles. Addition to that, this tutorial also shows how to automate and parallelize the process on Cheyenne supercomputers.

This tutorial assumes the basic knowledge on bash script language and that `AnEn` package has already been successfully installed. More information of how to install `AnEn` on Cheyenne can be found [here](https://weiming-hu.github.io/AnalogsEnsemble/2019/02/17/build-on-cheyenne.html).

This tutorial also assumes that you have already built the AnEn tools. Instructions for building the tools can be found [here](https://weiming-hu.github.io/AnalogsEnsemble/2019/02/17/build-on-cheyenne.html).

## Data Preparation and Goals

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

Original NAM forecast files are organized by day, cycle time, and lead time. Each file is a compilation of parameters at all available locations/grid points. However, data that AnEn requires have a [different format](https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html#forecasts). This format requires the file to have parameters, grid points, times, and lead times information included. Our goal is convert the model output to this format.

Since the total file size exceeds 5 TB, it would be a better practice to avoid a huge file, but to have it broken down to chunks. Therefore, the files are grouped by month.

## Scripts

I have prepared two scripts. The first script is the resource PBS script. This script does the following tasks:

- Identifies which folders are currently being processed by search for a lock file and which folders have already been processed by searching for the expected output data file;
- Selects *only one* folder that has not yet been processed;
- Untars tarballs in a temporary folder;
- Convert submessages to independent messages using `grib_copy` ([reference: grib_copy example 4](https://confluence.ecmwf.int/pages/viewpage.action?pageId=52462916#GRIBtoolsexamples-grib_copyexamples));
- Converts grb2 files to NetCDF files;
- Computes and adds wind direction and speed fields to the NetCDF file;
- Exits normally.

```
#!/bin/bash

# The name of the task
#PBS -N process_each_month

# The project account
#PBS -A MY.PROJECT.ACCOUNT

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
#PBS -M my.email@server.com

# These are the available folders. The folder names are also going to be the names of NetCDF files.
declare -a arr=("200810" "200811" "200812" "200901" "200902" "200903" "200904" "200905" "200906" "200907" "200908" "200909" "200910" "200911" "200912" "201001" "201002" "201003" "201004" "201005" "201006" "201007" "201008" "201009" "201010" "201011" "201012" "201101" "201102" "201103" "201104" "201105" "201106" "201107" "201108" "201109" "201110" "201111" "201112" "201201" "201202" "201203" "201204" "201205" "201206" "201207" "201208" "201209" "201210" "201211" "201212" "201301" "201302" "201303" "201304" "201305" "201306" "201307" "201308" "201309" "201310" "201311" "201312" "201401" "201402" "201403" "201404" "201405" "201406" "201407" "201408" "201409" "201410" "201411" "201412" "201501" "201502" "201503" "201504" "201505" "201506" "201507" "201508" "201509" "201510" "201511" "201512" "201601" "201602" "201603" "201604" "201605" "201606" "201607" "201608" "201609" "201610" "201611" "201612" "201701" "201702" "201703" "201704" "201705" "201706" "201707" "201708" "201709" "201710" "201711" "201712" "201801" "201802" "201803" "201804" "201805" "201806" "201807")

# Define the configuration file for gribConverter.
# The file can be found at 
# https://github.com/Weiming-Hu/AnalogsEnsemble/blob/master/apps/app_gribConverter/example/commonConfig.cfg
#
converterConfig=/glade/u/home/wuh20/scratch/data/forecasts/forecasts.cfg

# Define the output destination
destDir=/glade/u/home/wuh20/flash/forecasts_new/

# Define the lock file name
lockFile=.lock

for month in "${arr[@]}"; do
    # This is the data folder
    monthDir=/glade/u/home/wuh20/scratch/data/forecasts/$month
    
    # Whether this folder has already been processed
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
    
    # Create a folder to store the original files
    if [ ! -d original-extract-files ]; then
        echo Create folder for original extract files ...
        mkdir original-extract-files
    fi
    
    # Unpack tar files
    echo Extracting from tar files ...
    if [ -f log_extract  ]; then
        rm log_extract
    fi

    for tarFile in *.g2.tar; do
        tar --skip-old-files -xvf $tarFile -C original-extract-files >> log_extract
    done
    
    echo flattening messages with submessages ...
    for file in `ls original-extract-files`; do
        if [ ! -f $file ]; then
            /glade/u/home/wuh20/github/AnalogsEnsemble/dependency/install/bin/grib_copy original-extract-files/$file $file
        fi
    done
    
    # Convert grb2 files
    echo Converting grb2 files ...
    if [ ! -f $month-original.nc ]; then
        /glade/u/home/wuh20/github/AnalogsEnsemble/output/bin/gribConverter -c $converterConfig --folder ./ -o $month-original.nc -v 3 > log_converter
    fi

    # Add wind fields
    echo Adding wind fields ...
    if [ ! -f $month\.nc ]; then
        /glade/u/home/wuh20/github/AnalogsEnsemble/output/bin/windFieldCalculator --file-in $month-original.nc --file-type Forecasts --file-out $month\.nc -U 1000IsobaricInhPaU -V 1000IsobaricInhPaV --dir-name 1000IsobaricInhPaDir --speed-name 1000IsobaricInhPaSpeed -v 3 > log_wind
    fi

    # Move the data file elsewhere
    echo Moving data to $destDir
    mv $month\.nc $destDir
    
    # Cleaning
    echo Cleaning ...
    rm -rf original-extract-files
    rm $month-original.nc
    rm *.grb2
    
    echo Releasing the folder lock
    rm $lockFile
   
    # Each job only process one month
    echo Finished processing month $month
    exit 0
done
```

The first pbs script pretty much defines all the tasks that should be done. However, tasks for each month are entirely independent from each other and can be fully parallelized. Therefore, I decided that each task only processes one folder instead of continuing to the next folder available to avoid confusion between tasks. The following script simply deal with batch submitting the tasks to Cheyenne scheduler.

We have another problem here that if two tasks are started simultaneously, there is a slight possibility that they will process the same folder and the folder lock mechanism based on file creating might not work. A simple workaround for that is to ensure submitting a new job when there is no queueing tasks meaning all tasksing have been started.

```
#!/bin/bash

# Define the total number of jobs to create.
totalJobs=118

# Define the counter start.
submittedJobs=0

while true; do
    # Get the number of queued jobs by looking at the queue status looking for the symbols
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

By the completion of the scripts, we would have the following files in our destination folder:

```
> ls
200810.nc  200910.nc  201010.nc  201110.nc  201210.nc  201310.nc  201410.nc  201510.nc	201610.nc  201710.nc
200811.nc  200911.nc  201011.nc  201111.nc  201211.nc  201311.nc  201411.nc  201511.nc	201611.nc  201711.nc
200812.nc  200912.nc  201012.nc  201112.nc  201212.nc  201312.nc  201412.nc  201512.nc	201612.nc  201712.nc
200901.nc  201001.nc  201101.nc  201201.nc  201301.nc  201401.nc  201501.nc  201601.nc	201701.nc  201801.nc
200902.nc  201002.nc  201102.nc  201202.nc  201302.nc  201402.nc  201502.nc  201602.nc	201702.nc  201802.nc
200903.nc  201003.nc  201103.nc  201203.nc  201303.nc  201403.nc  201503.nc  201603.nc	201703.nc  201803.nc
200904.nc  201004.nc  201104.nc  201204.nc  201304.nc  201404.nc  201504.nc  201604.nc	201704.nc  201804.nc
200905.nc  201005.nc  201105.nc  201205.nc  201305.nc  201405.nc  201505.nc  201605.nc	201705.nc  201805.nc
200906.nc  201006.nc  201106.nc  201206.nc  201306.nc  201406.nc  201506.nc  201606.nc	201706.nc  201806.nc
200907.nc  201007.nc  201107.nc  201207.nc  201307.nc  201407.nc  201507.nc  201607.nc	201707.nc  201807.nc
200908.nc  201008.nc  201108.nc  201208.nc  201308.nc  201408.nc  201508.nc  201608.nc	201708.nc
200909.nc  201009.nc  201109.nc  201209.nc  201309.nc  201409.nc  201509.nc  201609.nc	201709.nc
```

And each file has the correct format for AnEn computation.

```
> ncdump -h 201801.nc 
netcdf \201801 {
dimensions:
	num_parameters = 17 ;
	num_chars = 50 ;
	num_stations = 262792 ;
	num_times = 31 ;
	num_flts = 53 ;
variables:
	char ParameterNames(num_parameters, num_chars) ;
	double ParameterWeights(num_parameters) ;
	char ParameterCirculars(num_parameters, num_chars) ;
	char StationNames(num_stations, num_chars) ;
	double Xs(num_stations) ;
	double Ys(num_stations) ;
	double Times(num_times) ;
	double FLTs(num_flts) ;
	double Data(num_flts, num_times, num_stations, num_parameters) ;
}
```
