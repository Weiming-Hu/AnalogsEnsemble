#!/usr/bin/env bash

# Declare the folder names where the data are
declare -a arr=("201701" "201702" "201703" "201704" "201705" \
    "201706" "201707" "201708" "201709" "201710" "201711" \
    "201712" "201801" "201802" "201803" "201804" "201805" \
    "201806" "201807" )

dataRootFrom=/home/graduate/wuh20/geolab_storage_V3/data/NAM/NCEI
dataRootTo=/media/wuh20/ExFat-Hu/final

for i in "${arr[@]}"
do
    if [ -f $dataRootTo/$i.nc ]; then
        echo $dataRootTo/$i.nc exists. Skip.
    else
        echo Generating $dataRootTo/$i.nc
        gribConverter -c commonConfig.cfg \
            --output $dataRootTo/$i.nc \
            --folder $dataRootFrom/forecasts/$i/extract/
    fi
done
