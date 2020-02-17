#!/bin/bash
###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This is the script to install NetCDF C++-4 API.
#
# Normally, you can just install the libraries using the following command:
#
# apt install libnetcdf-dev libnetcdf-c++4-dev
#
# However, if your NetCDF C++-4 version is lower than 4.3, some functions are not
# exported to the C++ API, e.g. NcFile::close(). This will create problem during
# CAnEnIO compilation because CAnEnIO needs the function. So then you need to build
# the latest NetCDF C++4 API.
#
# I'm downloading the 4.3.0 version rather than the latest version because there
# is known issues with the later versions when compiled with NetCDF newer versions.
#
# Make sure you already have installed NetCDF.
#

# Create a folder for NetCDF C++-4 API
mkdir ~/netcdf-c++4
cd ~/netcdf-c++4

# Download the release file and extract
wget https://github.com/Unidata/netcdf-cxx4/archive/v4.3.0.tar.gz -O netcdf-cxx4
tar xzf netcdf-cxx4
cd netcdf-cxx4-4.3.0

# Carry out-of-tree build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=~/netcdf-c++4 ..

# Build the project in parallel with the -j 16 option. 16 is the number of cores.
make -j 16

# Test installation
make check

# Install
make install

# Print out message
echo "
########################################################################################
#                                                                                      #
# Now, the latest version of NetCDF C++4 APIs have been installed to ~/netcdf-c++4     #
# If you want cmake to be able to find it, you can adding the following arguments      #
#                                                                                      #
#          cmake -DCMAKE_PREFIX_PATH=~/netcdf-c++4 [your extra arguments] ..           #
#                                                                                      #
# If you have multiple paths in the prefix path argument, separate them with ; and     #
# surround them with double quotes.                                                    #
#                                                                                      #
########################################################################################
"

