# Build Tensorflow C++ library from source to local folder
I am using Tensorflow 1.6, CUDA 8.0, and cuDNN 6.0

Special instructions or notes are given in parentheses, do not copy them

If you already have the library, feel free to use your own library

## Instructions:

1. Install dependencies

```
sudo apt-get install autoconf automake libtool curl make g++ unzip  (Protobuf Dependencies)
sudo apt-get install python-numpy swig python-dev python-wheel      (TensorFlow Dependencies)
```

2. Clone Tensorflow source

```
git clone https://github.com/tensorflow/tensorflow                 
cd tensorflow
git checkout (OPTIONAL other version e.g. v1.6.0)
bash tensorflow/contrib/makefile/download_dependencies.sh (then install dependencies separately)
```

3. Append the following to tensorflow/BUILD file:

```
cc_binary(
    name = "libtensorflow_all.so",
    linkshared = 1,
    linkopts = ["-Wl,--version-script=tensorflow/tf_version_script.lds"], # Remove this line if you are using MacOS
    deps = [
        "//tensorflow/core:framework_internal",
        "//tensorflow/core:tensorflow",
        "//tensorflow/cc:cc_ops",
        "//tensorflow/cc:client_session",
        "//tensorflow/cc:scope",
        "//tensorflow/c:c_api",
    ],
)
```

4. Build the shared library (locally)

```
./configure (I use default settings)
bazel build --config=opt --config=cuda --config=monolithic tensorflow:libtensorflow_all.so (append '-j 4' or lower number to limit computational resources)
```

5. Organize Tensorflow library in local folder

```
cd ~/tensorflow
mkdir -p local/lib
cp bazel-bin/tensorflow/libtensorflow_all.so local/lib 
mkdir -p local/include/google/tensorflow
cp -r tensorflow local/include/google/tensorflow
cp bazel-genfiles/tensorflow/core/framework/*.h local/include/google/tensorflow/tensorflow/core/framework
cp bazel-tensorflow/tensorflow/core/kernels/*.h local/include/google/tensorflow/tensorflow/core/kernels
cp bazel-genfiles/tensorflow/core/lib/core/*.h local/include/google/tensorflow/tensorflow/core/lib/core
cp bazel-genfiles/tensorflow/core/protobuf/*.h local/include/google/tensorflow/tensorflow/core/protobuf
cp bazel-genfiles/tensorflow/core/util/*.h local/include/google/tensorflow/tensorflow/core/util
cp bazel-genfiles/tensorflow/cc/ops/*.h local/include/google/tensorflow/tensorflow/cc/ops
cp -r third_party local/include/google/tensorflow/
```

## Note:
* In case of errors in Bazel build, downgrade Bazel to lower version

```
curl -LO "https://github.com/bazelbuild/bazel/releases/download/0.11.1/bazel_0.11.1-linux-x86_64.deb" 
$ sudo dpkg -i bazel_*.deb
```

* Before raising any issues, be sure to checkout the **Issues** in the referenced Github repos. You should be able to identify similar issue faced by others.

## Reference:
[Official Tensorflow guide](https://www.tensorflow.org/install/install_sources)

[Tensorflow-cmake](https://github.com/cjweeks/tensorflow-cmake)


