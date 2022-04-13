FROM ubuntu:18.04
LABEL maintainer="weiminghu@ucsd.edu"
LABEL description="A Linux Distribution of Parallel Analog Ensemble"

COPY . /root/PAnEn

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
    gcc g++ cmake libcppunit-dev doxygen graphviz \
    libnetcdf-c++4-dev libnetcdf-dev libeccodes-dev \
    openmpi-bin libopenmpi-dev git wget \
    libboost-all-dev make r-base && rm -rf /var/lib/apt/lists/*
RUN DEBIAN_FRONTEND=noninteractive && apt-get autoremove && apt-get autoclean

ENV LD_LIBRARY_PATH=/libs
ENV CPLUS_INCLUDE_PATH=/libs/include

#########################
# Default configuration #
#########################

WORKDIR /root/PAnEn/build_default
RUN cmake \
    -DCMAKE_INSTALL_PREFIX=../release_default \
    -DCMAKE_INSTALL_RPATH=../release_default/lib ..
RUN make install -j 4

#######
# MPI #
#######

WORKDIR /root/PAnEn/build_mpi
RUN cmake -DENABLE_MPI=ON -DENABLE_OPENMP=OFF \
    -DCMAKE_INSTALL_PREFIX=../release_mpi \
    -DCMAKE_INSTALL_RPATH=../release_mpi/lib ..
RUN make install -j 4

#############
# Deep AnEn #
#############

WORKDIR /root/torch
RUN wget -O libtorch.zip https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.11.0%2Bcpu.zip
RUN unzip libtorch.zip

WORKDIR /root/PAnEn/build_torch
RUN cmake -DCMAKE_PREFIX_PATH=/root/torch/libtorch \
    -DCMAKE_INSTALL_PREFIX=/root/PAnEn/release_torch \
    -DCMAKE_INSTALL_RPATH="/root/PAnEn/release_torch/lib;/root/torch/libtorch/lib" \
    -DENABLE_AI=ON -DBUILD_SHARED_LIBS=ON ..
RUN make install -j 4

WORKDIR /root
CMD ["/bin/bash"]

