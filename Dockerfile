FROM ubuntu:18.04
LABEL maintainer="weiming@psu.edu"
LABEL description="A Linux Distribution of Parallel Analog Ensemble"

COPY . /PAnEn_source

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
    gcc g++ cmake libcppunit-dev doxygen libnetcdf-c++4-dev libnetcdf-dev \
    libeccodes-dev openmpi-bin libopenmpi-dev git \
    libboost-all-dev make r-base && rm -rf /var/lib/apt/lists/*
RUN DEBIAN_FRONTEND=noninteractive && apt-get autoremove && apt-get autoclean

ENV LD_LIBRARY_PATH=/libs
ENV CPLUS_INCLUDE_PATH=/libs/include

WORKDIR /PAnEn_source/buildC
RUN CC=gcc CXX=g++ cmake -DENABLE_MPI=ON ..
RUN make install

WORKDIR /PAnEn_source/buildR
RUN Rscript -e "install.packages(c('Rcpp', 'BH'))"
RUN CC=gcc CXX=g++ cmake -DINSTALL_RAnEn=ON ..
RUN make install

WORKDIR /torch
RUN wget -O libtorch.zip https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.11.0%2Bcpu.zip
RUN unzip libtorch.zip

WORKDIR /root

CMD ["/bin/bash"]
