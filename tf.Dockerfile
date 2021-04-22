# Note: All of the links below are copied over from the source code.
# The dependencies should be self-contained

FROM nvidia/cudagl:9.0-devel-ubuntu16.04
LABEL maintainer "NVIDIA CORPORATION <cudatools@nvidia.com>"

ENV CUDNN_VERSION 7.6.4.38

LABEL com.nvidia.cudnn.version="${CUDNN_VERSION}"

RUN apt-get update && apt-get install -y --no-install-recommends \
    libcudnn7=$CUDNN_VERSION-1+cuda9.0 \
    libcudnn7-dev=$CUDNN_VERSION-1+cuda9.0 \
    && apt-mark hold libcudnn7 && \
    rm -rf /var/lib/apt/lists/*

# Add nvidia driver settings
ENV NVIDIA_VISIBLE_DEVICES ${NVIDIA_VISIBLE_DEVICES:-all}
ENV NVIDIA_DRIVER_CAPABILITIES ${NVIDIA_DRIVER_CAPABILITIES:+$NVIDIA_DRIVER_CAPABILITIES,}graphics

RUN apt-get update && apt-get install -y --no-install-recommends \
     build-essential curl git cmake unzip autoconf autogen automake \
     libtool mlocate zlib1g-dev g++-6 python python-numpy python-dev \
     python3-pip python3-wheel wget gzip openjdk-8-jdk make swig\
     bash-completion  python-wheel libopencv-dev && \
    rm -rf /var/lib/apt/lists/*
RUN updatedb

# Bazel
RUN curl -LO "https://github.com/bazelbuild/bazel/releases/download/0.11.1/bazel_0.11.1-linux-x86_64.deb"
RUN dpkg -i bazel_*.deb && rm -rf bazel_*.deb

# Protobuf
RUN wget https://github.com/google/protobuf/archive/396336eb961b75f03b25824fe86cf6490fb75e3a.tar.gz && \
    tar -xvf 396336eb961b75f03b25824fe86cf6490fb75e3a.tar.gz && \
    cd protobuf-396336eb961b75f03b25824fe86cf6490fb75e3a && \
    ./autogen.sh && \
    ./configure --prefix=/usr && \
    make install && \
    cd .. && \
    rm -rf protobuf-* && \
    rm -rf 396336eb961b75f03b25824fe86cf6490fb75e3a.tar.gz

# Eigen
RUN wget https://mirror.bazel.build/bitbucket.org/eigen/eigen/get/14e1418fcf12.tar.gz && \
    tar -xvf 14e1418fcf12.tar.gz && \
    mv eigen-eigen-14e1418fcf12 eigen3-tf && \
    rm -rf 14e1418fcf12.tar.gz

ENV CUDA_TOOLKIT_PATH=/usr/local/cuda-9.0
ENV CUDNN_INSTALL_PATH=/usr/lib/x86_64-linux-gnu
ENV TF_NEED_GCP=0
ENV TF_NEED_CUDA=1
ENV TF_CUDA_VERSION=9.0
ENV TF_CUDA_COMPUTE_CAPABILITIES=6.1
ENV TF_CUDNN_VERSION=7.6.4
ENV CC_OPT_FLAGS="-march=native"
ENV GCC_HOST_COMPILER_PATH=/usr/bin/gcc

# Prompt for settings
RUN git clone https://github.com/tensorflow/tensorflow.git && \
    cd tensorflow && \
    git checkout v1.6.0 && \
    ./configure && \
    bazel build --config=opt --config=cuda --config=monolithic //tensorflow:libtensorflow_cc.so && \
    mkdir -p                /usr/local/include/google/tensorflow && \
    cp -r bazel-genfiles/*  /usr/local/include/google/tensorflow/  && \
    cp -r tensorflow        /usr/local/include/google/tensorflow/ && \
    find                    /usr/local/include/google/tensorflow -type f  ! -name "*.h" -delete && \
    cp -r third_party       /usr/local/include/google/tensorflow/ && \
    cp bazel-bin/tensorflow/libtensorflow_cc.so /usr/local/lib && \
    mkdir -p                /usr/local/include/google/tensorflow/external/nsync/public && \
    cp -r bazel-tensorflow/external/nsync/public/* /usr/local/include/google/tensorflow/external/nsync/public

WORKDIR /
RUN rm -rf tensorflow
