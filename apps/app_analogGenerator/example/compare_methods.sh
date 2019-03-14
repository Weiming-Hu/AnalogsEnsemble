#!/usr/bin/env bash

# This script shows how to use the utilities simialrityCalculator, analogSelector, and analogGnerator.
# The script first generate similarity from similarityCalculator and then feed the output into analogSelector
# to generate analogs. Then the script shows how to do these two steps altogether with analogGenerator.
#
# The results, simialrity and analogs, from the two methods are compared, and they should be identical.
# You can check this by reading the final output messages.
#
rm -f map.txt
rm -f sim_sep.nc
rm -f sim_ctn.nc
rm -f analogs_sep.nc
rm -f analogs_ctn.nc
rm -f analogs_sep_wo_mapping.nc

../../../output/bin/similarityCalculator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc --similarity-nc sim_sep.nc --test-start 0 0 9 0 --test-count 5 20 1 8 --search-start 0 0 0 0 --search-count 5 20 9 8 --time-match-mode 1 --mapping-txt map.txt -v 2

../../../output/bin/analogSelector --similarity-nc sim_sep.nc --observation-nc ../../../tests/Data/test_observations.nc --analog-nc analogs_sep.nc --members 5 --mapping-txt map.txt -v 2

../../../output/bin/analogSelector --similarity-nc sim_sep.nc --observation-nc ../../../tests/Data/test_observations.nc --analog-nc analogs_sep_wo_mapping.nc --members 5 -v 2

../../../output/bin/analogGenerator --test-forecast-nc ../../../tests/Data/test_forecasts.nc --search-forecast-nc ../../../tests/Data/test_forecasts.nc --observation-nc ../../../tests/Data/test_observations.nc --similarity-nc sim_ctn.nc --test-start 0 0 9 0 --test-count 5 20 1 8 --search-start 0 0 0 0 --search-count 5 20 9 8 --time-match-mode 1 --analog-nc analogs_ctn.nc --members 5 -v 1

diff analogs_sep.nc analogs_ctn.nc && diff analogs_sep_wo_mapping.nc analogs_ctn.nc
status=$?
if test $status -eq 0
then
    echo Passed! Analog results from two methods are the same.
else
    echo Failed! Analog results from two methods are NOT identical.
fi

diff sim_sep.nc sim_ctn.nc
status=$?
if test $status -eq 0
then
    echo Passed! Similarity results from two methods are the same.
else
    echo Failed! Similarity results from two methods are NOT identical.
fi

rm -f map.txt
rm -f sim_sep.nc
rm -f sim_ctn.nc
rm -f analogs_sep.nc
rm -f analogs_ctn.nc
rm -f analogs_sep_wo_mapping.nc
