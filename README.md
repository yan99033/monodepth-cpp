# monodepth-cpp
Tensorflow C++ implementation for single image depth estimation
<p align="center">
 <img src="https://github.com/yan99033/monodepth-cpp/blob/master/preview/monodepth_preview.gif" width="612" height="370">
</p>

The original work is implemented in Python, [click here](https://github.com/mrharicot/monodepth) to go to their repo. Please cite their work if your find it helpful.

It is using pointer-to-implementation technique, so that you can use it in your project without worrying the actual implementation. Refer to [src/inference_monodepth.cpp](https://github.com/yan99033/monodepth-cpp/tree/master/src/inference_monodepth.cpp) for more information

The C++ version is about 28fps, in comparison with Python's 13fps, tested with i7 processor and NVidia 1070 graphics laptop.

## Related repo
If you are looking for single-image relative depth prediction, feel free to check out my other repo, [MiDaS-cpp](https://github.com/yan99033/MiDaS-cpp), a PyTorch C++ implementation of MiDaS.


## Requirements
### Option 1
Run the Docker container
```
chmod +x run_tf_docker.sh
./run_tf_docker.sh
```
Note: You may have a better graphics card, and if that's so, go to [tf.Dockerfile](https://github.com/yan99033/monodepth-cpp/tree/master/tf.Dockerfile) line 55 and change the value of `TF_CUDA_COMPUTE_CAPABILITIES` (check out [Nvidia cuda-gpus](https://developer.nvidia.com/cuda-gpus) ).
### Option 2
1. Build Tensorflow library from source, [see here](https://github.com/yan99033/monodepth-cpp/tree/master/Tensorflow_build_instructions)
2. [Freeze the Tensorflow graph](https://github.com/yan99033/monodepth-cpp/tree/master/freeze_graph) (with known input and output names)


## Configure CMake project
* Make sure you have the compatible versions of Eigen, Protobuf, and Tensorflow (Mine: Eigen 3.3.4; Protobuf 2.6.1-1.3; Tensorflow 1.6)
* If you have a set of different paths to the libraries, modify the `.cmake` files in [CMakeModules](https://github.com/yan99033/monodepth-cpp/tree/master/CMakeModules)


## Build (static/shared) library
**NOTE:** If you use the Docker container, this github repo will be mounted in `/monodepth`. Therefore, run `cd monodepth` in the container first.

```
mkdir build && mkdir install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install ..
make && make install
```

You will be seeing 'include' and 'lib' folders in the 'install' folder, import them in your project

To test if Monodepth C++ is working properly,
```
cd build
./inference_monodepth
```

**NOTE:** Select either static or shared library in CMakeLists.txt, unless you want both of them

## Use (static/shared) library
In your own CMakeLists.txt project, do the following:

```
set(monodepth_INCLUDE_DIRS /path/to/monodepth-cpp/install/include)
INCLUDE_DIRECTORIES(
  ...
  monodepth_INCLUDE_DIRS
  ...
)

TARGET_LINK_LIBRARIES(awesome_exe /path/to/tensorflow/library/libtensorflow_cc.so) # Only if you are using the provided instructions
TARGET_LINK_LIBRARIES(awesome_exe /path/to/monodepth-cpp/install/lib/libmonodepth_static.a) # if you are using static library

```

## Personal projects that use monodepth-cpp
1. [CNN-SVO](https://github.com/yan99033/CNN-SVO)
<p align="center">
 <img src="https://github.com/yan99033/monodepth-cpp/blob/master/preview/kitti_preview.gif" width="723" height="224">
 <img src="https://github.com/yan99033/monodepth-cpp/blob/master/preview/robotcar_preview.gif" width="723" height="224">
</p>

2. Reproducing [Deep virtual stereo odometry](https://vision.in.tum.de/research/vslam/dvso)

<p align="center">
 <img src="https://github.com/yan99033/monodepth-cpp/blob/master/preview/dvso_kitti_preview.gif" width="723" height="281">
 <img src="https://github.com/yan99033/monodepth-cpp/blob/master/preview/dvso_final_map.gif" width="723" height="334">
</p>

**NOTE:** Because DVSO uses both the left disparity and the right disparity outputs (for left-right consistency check), it requires some modifications in the source code to enable the disparities outputs.



