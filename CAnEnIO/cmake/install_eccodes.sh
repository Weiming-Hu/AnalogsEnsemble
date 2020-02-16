#!/bin/bash
###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This is the script to install Eccodes. You might need sudo to run this.
#
# This script is designed to be run on CI machines. But this can also be run manually
# to install eccodes. You might need sudo permission to run this script, especially the
# last command to install the library to your system.
#

# This is the version to download
export ECCODES_VERSION=2.16.0

# Download the tarball file
wget "https://confluence.ecmwf.int/download/attachments/45757960/eccodes-$ECCODES_VERSION-Source.tar.gz?api=v2"

# Extract the tarball file
tar -xzf eccodes-$ECCODES_VERSION-Source.tar.gz

# Carry out out-of-tree build
mkdir build
cd build
cmake -DENABLE_PYTHON=OFF -DENABLE_FORTRAN=OFF ..

# Build the project in parallel with the -j 16 option. 16 is the number of cores.
make -j 16

# Test installation
ctest

# Install to system. This is the place you might need sudo permission
make install
