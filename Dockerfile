FROM ubuntu:18.04
LABEL maintainer="weiming@psu.edu"
LABEL description="A Linux Distribution of the C++ Program - Parallel Ensemble Forecasts"

COPY . /PEF_source

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y gcc cmake \
    libnetcdf-dev git make r-base  && apt-get clean  && rm -rf /var/lib/apt/lists/*

WORKDIR /PEF_source
RUN mkdir build

WORKDIR /PEF_source/buildC
RUN CC=gcc CXX=g++ cmake -DCMAKE_BUILD_TESTS=ON ..
RUN make -j 16 install

WORKDIR /PEF_source/buildR
RUN Rscript -e "install.packages(c('Rcpp', 'BH'))"
RUN CC=gcc CXX=g++ cmake -DINSTALL_RAnEn=ON ..
RUN make -j 16 install

CMD ["/bin/bash"]
