#!/usr/bin/env bash

# This script shows how to use the multiple input file feature of simialrity calculator and analog selector, and test
# the results with the file aggregator and the analog generators.
#

rm sims_cnt.nc || true
rm analogs_cnt.nc || true
rm sims_sep.nc || true
rm analogs_sep.nc || true
rm search.nc || true
rm obs.nc || true

verbose=2

similarityCalculator \
    --search-forecast-nc ~/geolab_storage_V3/data/NAM/NCEI/201601.nc ~/geolab_storage_V3/data/NAM/NCEI/201602.nc \
    --observation-nc ~/geolab_storage_V3/data/NAM/NCEI/analysis_new/201601.nc ~/geolab_storage_V3/data/NAM/NCEI/analysis_new/201602.nc \
    --test-forecast-nc ~/geolab_storage_V3/data/NAM/NCEI/201701.nc \
    --search-start 0 0 0 0 0 0 0 0 --search-count 5 10 31 4 5 10 29 4 \
    --obs-start 0 0 0 0 0 0 --obs-count 5 10 496 5 10 464 \
    --test-start 0 0 0 0 --test-count 5 10 3 4 \
    --search-along 2 --obs-along 2 \
    --similarity-nc sims_sep.nc \
    --verbose $verbose

analogSelector \
    --observation-nc ~/geolab_storage_V3/data/NAM/NCEI/analysis_new/201601.nc ~/geolab_storage_V3/data/NAM/NCEI/analysis_new/201602.nc \
    --obs-start 0 0 0 0 0 0 --obs-count 5 10 496 5 10 464 \
    --similarity-nc sims_sep.nc \
    --analog-nc analogs_sep.nc \
    --obs-along 2 \
    --members 4 \
    --verbose $verbose

fileAggregate \
    --type Observations \
    -i ~/geolab_storage_V3/data/NAM/NCEI/analysis_new/201601.nc ~/geolab_storage_V3/data/NAM/NCEI/analysis_new/201602.nc \
    --start 0 0 0 0 0 0 --count 5 10 496 5 10 464 \
    -o obs.nc \
    --along 2 \
    --verbose $verbose

fileAggregate \
    --type Forecasts \
    -i ~/geolab_storage_V3/data/NAM/NCEI/201601.nc ~/geolab_storage_V3/data/NAM/NCEI/201602.nc \
    --start 0 0 0 0 0 0 0 0 --count 5 10 31 4 5 10 29 4 \
    --along 2 \
    -o search.nc \
    --verbose $verbose

analogGenerator \
    --search-forecast-nc search.nc \
    --observation-nc obs.nc \
    --analog-nc analogs_cnt.nc \
    --members 4 \
    --test-forecast-nc ~/geolab_storage_V3/data/NAM/NCEI/201701.nc \
    --test-start 0 0 0 0 --test-count 5 10 3 4 \
    --similarity-nc sims_cnt.nc \
    --verbose $verbose

diff analogs_cnt.nc analogs_sep.nc && diff sims_cnt.nc sims_sep.nc
status=$?
if test $status -eq 0
then
    echo Passed! Analog results from two methods are the same.
else
    echo Failed! Analog results from two methods are NOT identical.
fi


rm sims_cnt.nc || true
rm analogs_cnt.nc || true
rm sims_sep.nc || true
rm analogs_sep.nc || true
rm search.nc || true
rm obs.nc || true

