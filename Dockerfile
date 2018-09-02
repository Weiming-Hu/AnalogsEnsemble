FROM ubuntu:18.04
MAINTAINER Weiming Hu <weiming@psu.edu>
LABEL description="A Linux Distribution of the C++ Program - Parallel Ensemble Forecasts"

COPY . /PEF_source

RUN apt-get update && apt-get install -y \
  gcc \
  cmake \
  libnetcdf-dev \
  git \
  make

WORKDIR /PEF_source
RUN mkdir build

WORKDIR /PEF_source/build
RUN CC=gcc CXX=g++ cmake -DCMAKE_BUILD_TESTS=ON ..
RUN make -j 16 install

CMD ["/bin/bash"]
