#!/bin/bash

# For analog generation
totalJobs=50
jobFile="/glade/u/home/wuh20/work/SEA2019/scripts/01_analog_generation.pbs"

# For analog reshape
#totalJobs=31
#jobFile="/glade/u/home/wuh20/work/SEA2019/scripts/02_analog_reshape.pbs"

for i in $(seq 0 $(($totalJobs-1))); do
    echo qsub -v index=$i $jobFile

    # Submit the job with a specific id as an argument
    # This id will be used in the job scrip to determine
    # which configuration file to read
    #
    qsub -v index=$i $jobFile
done
