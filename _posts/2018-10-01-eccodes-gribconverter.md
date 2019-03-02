---
layout: post
title: How to use Eccodes Tools and the gribConverter Tool
tags:
  - tutorial
  - gribConverter
---


<!-- vim-markdown-toc GitLab -->

* [Introduction](#introduction)
* [Preparation](#preparation)
        * [gribConverter](#gribconverter)
        * [Eccodes](#eccodes)
* [Usage](#usage)
        * [Extract Time and FLTs](#extract-time-and-flts)
        * [Specifying Parameters](#specifying-parameters)
* [Example](#example)
* [Caveat](#caveat)
* [Summary](#summary)

<!-- vim-markdown-toc -->

## Introduction

The `gribConverter` tool provided in this package offers the convenience for converting GRB2 files to NetCDF files. The produced NetCDF files should be ready to be read into Analog computing tools like `similarityCalculator`. This post demonstrates how to use `Eccodes` tools and the `gribConverter` together to complete the file conversion task.

## Preparation

#### gribConverter

`gribConverter` is by default not built, because it depends on the [ECMWF ecCodes library](https://confluence.ecmwf.int/display/ECC/ecCodes+Home). 

To build `gribConverter`, you can specify the parameter to `CMake`.

```
cmake -DBUILD_GRIBCONVERTER=ON ..

# If the eccodes library cannot be found, try to tell where it is explicitly
cmake -DBUILD_GRIBCONVERTER=ON -CMAKE_PREFIX_PATH=<path to eccodes> ..
```

The library `eccodes` will be automatically built by default if `gribConverter` is built. However, in cases of failure during building `eccodes`, please install the library using your preferred package management tool, for example, `HomeBrew` on Mac OS or `apt-get` on Debian.

More details please see [the documentation](https://weiming-hu.github.io/AnalogsEnsemble/#installation)

#### Eccodes

Please install [ECMWF ecCodes library](https://confluence.ecmwf.int/display/ECC/ecCodes+Home) using your preferred package management tools. Upon successful installation, you should have the following tools available:

- grib_ls
- grib_get_data
- ...

## Usage

I have prepared a GRB2 file downloaded from [NAM Forecast System](https://www.ncdc.noaa.gov/data-access/model-data/model-datasets/north-american-mesoscale-forecast-system-nam). It can be accessed [here](https://weiming.ddns.net/nextcloud/index.php/s/9QS4YJPDAirjtoM)(54.6 MB).

`gribConverter` reads from all files found in the folder specifed by arguments `folder` and `ext`. It assumes that the time and the Forecast Lead Time (FLT) information comes from the file name. In the case of the example data file, the file name is `nam_218_20180701_0000_000.grb2`. `20180701` is the date (time) information, `0000` is the model cycle time information, and `000` is the forecast lead time. Later on, we are going to extract the needed information using regular expression.

For parameter information, `gribConverter` will try to read them from the file according to the input arguments.

For station information, `gribConverter` automatically reads lat/lon or x/y from the file.

#### Extract Time and FLTs

In order to extract time information from the file name, we use the following regular expression.

```
.*nam_218_(\d{8})_\d{4}_\d{3}\.grb2$
```

`.*` means to match any character 0 or more times. `\d{8}` means to match numeric digit exactly 8 times. When it is enclosed by a pair of parenthesis, it shows that this string of number is what we need. In this case, the 8-digit number is what we need for time information. Sometimes, if the string is delimited, for example `2018-09-01`, you can use the option `--delimited` to solve the problem. The program should automatically detect the dilimiter.

Same story to extract FLT information.

```
.*nam_218_\d{8}_\d{4}_(\d{3})\.grb2$
```

We only changed the location of parenthesis, because the 3-digit number is what we need for the FLT information. Please pay attention to the options `flt-interval`. This is the number of seconds between two forecast lead times. For example, in the case of an hourly forecasts, this should be 60 * 60 = 3600.

**I recommend specifying the regular expression in a configuration file in stead of in commandline options to avoid worrying about escaping characters.**

#### Specifying Parameters

In order for the program to know exactly which parameters you want to extract from the GRB2 file and wirte into a NetCDF file, we need to specify three attributes of a parameter:

- pars-id
- levels
- level-types

These information can be found using the `eccodes` tool `grib_ls`.

To print out all the available parameters in the file, you can type the following commands:

```
> grib_ls -p paramId,level,typeOfLevel,name nam_218_20180701_0000_000.grb2 | head -20
nam_218_20180701_0000_000.grb2
paramId      level        typeOfLevel  name         
260074       0            meanSea      Pressure reduced to MSL 
260389       1            hybrid       Derived radar reflectivity 
260390       0            unknown      Maximum/Composite radar reflectivity 
3020         0            surface      Visibility  
3020         0            cloudTop     Visibility  
260190       1            hybrid       Blackadar mixing length scale 
156          0            unknown      Geopotential Height 
131          0            unknown      U component of wind 
132          0            unknown      V component of wind 
7001353      0            unknown      Ventilation Rate 
260065       0            surface      Wind speed (gust) 
0            0            unknown      unknown     
260238       5            isobaricInhPa  Geometric vertical velocity 
260238       10           isobaricInhPa  Geometric vertical velocity 
260238       20           isobaricInhPa  Geometric vertical velocity 
156          50           isobaricInhPa  Geopotential Height 
130          50           isobaricInhPa  Temperature 
157          50           isobaricInhPa  Relative humidity 
```

`-p paraId,level,typeOfLevel,name` tells the program to print out only these attributes. `| head -20` is optional, telling the program to print out only the first 10 lines.

You can also find more information for a variable using the [ECMWF GRIB Parameter database](http://apps.ecmwf.int/codes/grib/param-db/). This is a list of parameters that `grib_ls` will recognize.

You have two ways to specify parameters in `gribConverter`.

In commandline options:

```
# All options should be in order
--pars-id=156 131 --levels=0 0 ---level-types=unknown unknown
```

In a configuration file

```
# Parameter 156
pars-id = 156
levels = 0
level-types=unknown
parameters-new-name = 156_0

# Parameter 131
pars-id = 131
levels = 0
level-types=unknown
parameters-new-name = 131_0
```

** Multiple input on a single line is not supported in configuration files**

## Example

An working example can be found [here](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/apps/app_gribConverter/example).

- commonConfig.cfg is the common configuration file that all files share. It includes basic options like parameters, times, and flts;
- convertForecasts.sh is the script specifying the different options for files, includes `output` and `folder`.

## Caveat

Please note in the file `convertForecasts.sh` on [line 3](https://github.com/Weiming-Hu/AnalogsEnsemble/blob/2ab0a1a5049917a1c55ef258cc725bb63fe6b780/apps/app_gribConverter/example/convertForecasts.sh#L3), I specified the number of threads to be created in the program. It is NOT the case that the more threads you create the faster your program will run. This also depends on your network quality. Because when too many threads are created, it actually slows download the file transfer process. For my case where the maximum network speed can go up until ~30 MB/s, I found that when I created more than 5 threads, it will slow down the network. Therefore, I tested with creating 1, 2, 3, and 5 threads, and decided I should only create 3 which should be a fairly balanced point.

## Summary

This post explains the typical way to use the `girbConverter`. There are some options not covered in this article, please refered to `gribConverter --help`.

Please report any issues [here](https://github.com/Weiming-Hu/AnalogsEnsemble/issues). Thank you.

```
# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <weiming@psu.edu>
#         
# Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
# Department of Geography and Institute for CyberScience
# The Pennsylvania State University
```
