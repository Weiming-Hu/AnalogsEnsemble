#!/usr/bin/env bash

# Declare the folder names where the data are
declare -a arr=("201701" "201702" "201703" "201704" "201705" \
    "201706" "201707" "201708" "201709" "201710" "201711" \
    "201712" "201801" "201802" "201803" "201804" "201805" \
    "201806" "201807" )

dataRoot=/home/graduate/wuh20/geolab_storage_V3/data/NAM/NCEI

for i in "${arr[@]}"
do
    if [ -f $dataRoot/forecastsExtracted/$i.nc ]; then
        echo $dataRoot/forecastsExtracted/$i.nc exists. Skip.
    else
        echo Generating $dataRoot/forecastsExtracted/$i.nc
        gribConverter -c commonConfig.cfg \
            --output $dataRoot/forecastsExtracted/$i.nc \
            --folder $dataRoot/forecasts/$i/extract/
    fi
done
