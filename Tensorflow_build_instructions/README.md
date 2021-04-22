# Build Tensorflow C++ library from source to local folder
I use Tensorflow 1.6, CUDA 9.0, and cuDNN 7.6.
(Similar instructions can be found in the Dockerfile)

Most of the libraries should not be installed in system directory as they are dated. 

If you already have the library, feel free to use your own library

## Instructions:

Create a folder named `third_party`
```
mkdir third_party
cd third_party
```
### Install dependencies

```
apt install autoconf automake libtool curl make g++ unzip  (Protobuf Dependencies)
apt install python-numpy swig python-dev python-wheel      (TensorFlow Dependencies)
apt install wget openjdk-8-jdk
```

Bazel (using a different version might result in build error)
```
curl -LO "https://github.com/bazelbuild/bazel/releases/download/0.11.1/bazel_0.11.1-linux-x86_64.deb"
dpkg -i bazel_*.deb && rm -rf bazel_*.deb
```

Protobuf
```
wget https://github.com/google/protobuf/archive/396336eb961b75f03b25824fe86cf6490fb75e3a.tar.gz
tar -xvf 396336eb961b75f03b25824fe86cf6490fb75e3a.tar.gz
mv protobuf-396336eb961b75f03b25824fe86cf6490fb75e3a protobuf
cd protobuf
mkdir install
./autogen.sh
./configure --prefix=install
make install
cd ..
rm -rf 396336eb961b75f03b25824fe86cf6490fb75e3a.tar.gz
```

Eigen
```
wget https://mirror.bazel.build/bitbucket.org/eigen/eigen/get/14e1418fcf12.tar.gz
tar -xvf 14e1418fcf12.tar.gz
mv eigen-eigen-14e1418fcf12 eigen3-tf
rm -rf 14e1418fcf12.tar.gz
```

### Clone Tensorflow source

```
git clone https://github.com/tensorflow/tensorflow                 
cd tensorflow
git checkout v1.6.0
```

### Build the shared library (locally)

```
./configure (I use default settings)
    bazel build --config=opt --config=cuda --config=monolithic tensorflow:libtensorflow_cc.so
```

### Organize Tensorflow library in local folder
```
mkdir -p                local/lib
mkdir -p                local/include/google/tensorflow
cp -r bazel-genfiles/*  local/include/google/tensorflow/  
cp -r tensorflow        local/include/google/tensorflow/ 
find                    local/include/google/tensorflow -type f  ! -name "*.h" -delete 
cp -r third_party       local/include/google/tensorflow/ 
cp bazel-bin/tensorflow/libtensorflow_cc.so local/lib 
mkdir -p                local/include/google/tensorflow/external/nsync/public 
cp -r bazel-tensorflow/external/nsync/public/* local/include/google/tensorflow/external/nsync/public
```

## Note:
* Before raising any issues, be sure to checkout the **Issues** in the referenced Github repos. You should be able to identify similar issue faced by others.

## Reference:
[Official Tensorflow guide](https://www.tensorflow.org/install/install_sources)

[Tensorflow-cmake](https://github.com/cjweeks/tensorflow-cmake)


