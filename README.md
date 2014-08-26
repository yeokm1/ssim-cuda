ssim-cuda
=========

CUDA Program to measure the similarity between two videos using the OpenCV library and the structural similarity algorithm (SSIM). This is modified from the [gpu-basics-similarity tutorial](http://docs.opencv.org/doc/tutorials/gpu/gpu-basics-similarity/gpu-basics-similarity.html) of OpenCV. For the non-CUDA, pure CPU version, checkout my other [project](https://github.com/yeokm1/ssim).

##Usage
```bash
#ssim-cuda reference_video_file test_video_file reference_start_frame test_start_frame [numFramesToCompare]
ssim-cuda reference.avi test.avi 0 1
ssim-cuda reference.avi test.avi 5 13 1000
```

##Dependencies I used
1. OpenCV 2.4.9
2. Cmake 3.0.1
3. Visual Studio 2012 (VS2012 codenamed vc11) Update 4. Visual Studio Express should work too.
4. Nvidia Cuda 6.5

VS2013 has some problems compiling OpenCV 2.4.9 at this time but should have been fixed in the latest 3.0 alpha. Once the next stable version is released, you can use VS2013.

###Installing Dependencies

1. Install Visual Studio 2012 (VS2012 codenamed vc11). Install latest update which is "Update 4" at this time. Make sure to install VS2012 <b>before</b> CUDA drivers so the CUDA installer can install the VS Plugin properly.
2. [Download CUDA 6.5](https://developer.nvidia.com/cuda-downloads). Select custom install. Check everything except 3D Vision as I don't use it.  Let the installer override your current graphics drivers if yours is the older version.
3. [Download OpenCV 2.4.9](https://github.com/Itseez/opencv/releases) source zip. Do not use the installable exe as that does not come with CUDA support. Unzip to `C:\opencv`
4. [Download Cmake 3.0.1 Win32 installer](http://www.cmake.org/cmake/resources/software.html). During installation, add Cmake to system path of all users.

###Compiling OpenCV
1. Open CMake GUI as administrator so it access all directories.
2. Set source to `C:\opencv` and where-to-build to `C:\opencv\build`
3. Click "Configure", choose Visual Studio 11 2012, "Default native compilers".
4. Search for CUDA. For `CUDA_ARCH_BIN`, remove all the numbers except the number of your current GPU architecture. For example, I use a GeForce 650 which has a compute capability (CC) of 3.0. Use this [list](https://developer.nvidia.com/cuda-gpus) to look up the CC of your GPU. We remove other numbers to shorten compilation time by only compiling for the CC we want.
5. Empty out the `CUDA_ARCH_PTX` field as I don't use a virtual platform. Tick `CUDA_FAST_MATH`. Verify that `WITH_CUDA` and `BUILD_opencv_gpu` are also ticked. Check that `CUDA_TOOLKIT_ROOT_DIR` is correctly populated.
6. To speed up compilation even more, I untick `BUILD_TESTS` and `BUILD_PERF_TESTS`.
7. Click Generate.
8. Go to `C:\opencv\build` and open OPENCV.sln. Select Debug Build.
9. Click BUILD in the toolbar and BUILD ALL_BUILD. If VS asks to reload modules because they have been modified externally, just reload.
10. Select Release Build and repeat step 8.
11. Go to System Properties to set your path. Modify path by adding, `c:\opencv\build\bin\DEBUG;c:\opencv\build\bin\RELEASE`
