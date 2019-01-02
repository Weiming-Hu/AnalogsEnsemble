#!/usr/bin/env bash

# This script shows how to use the analogGenerator with and without the standard deviation
# file as input. The script compares the result analogs from two method. They should be 
# identical. You can check the standard output message for this.
#

rm -f analogs_sds.nc
rm -f analogs.nc
rm -f sds.nc

verbose=1

../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc -v $verbose --test-start 0 0 9 0 --test-count 5 20 1 8 --search-start 0 0 0 0 --search-count 5 20 9 8 --time-match-mode 1 --analog-nc analogs.nc --members 5

../../../output/bin/standardDeviationCalculator -i ../../../tests/Data/test_forecasts.nc -o sds.nc -v $verbose --start 0 0 0 0 --count 5 20 9 8

../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc -v $verbose --test-start 0 0 9 0 --test-count 5 20 1 8 --search-start 0 0 0 0 --search-count 5 20 9 8 --time-match-mode 1 --analog-nc analogs_sds.nc --members 5 --sds-nc sds.nc

diff analogs.nc analogs_sds.nc
status=$?
if test $status -eq 0
then
    echo Passed! Analog results from two methods are the same.
else
    echo Failed! Analog results from two methods are NOT identical.
fi

rm -f analogs_sds.nc
rm -f analogs.nc
rm -f sds.nc
