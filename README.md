ssim-cuda
=========

CUDA Program to measure the similarity between two videos using the OpenCV library and the structural similarity algorithm (SSIM). This is modified from the [gpu-basics-similarity tutorial](http://docs.opencv.org/doc/tutorials/gpu/gpu-basics-similarity/gpu-basics-similarity.html) of OpenCV. For the non-CUDA, pure CPU version, check out my other [project](https://github.com/yeokm1/ssim).

##Usage
```bash
#ssim-cuda reference_video_file test_video_file reference_start_frame test_start_frame delayBetweenFrames [numFramesToCompare]
ssim-cuda reference.avi test.avi 0 1 0
ssim-cuda reference.avi test.avi 5 13 10 1000
```

delayBetweenFrames is the ms delay between frames. If delay is 0, video will not be shown on screen but calculation will be a lot faster. A value of 10 is recommended.

##Dependencies I used
1. OpenCV 2.4.9
2. Cmake 3.0.1
3. Visual Studio 2012 (VS2012 codenamed vc11) Update 4. Visual Studio Express should work too.
4. Nvidia Cuda 6.5 64-bit
5. Compiled on Windows 8.1 Pro x64

VS2013 has some problems compiling OpenCV 2.4.9 at this time but should have been fixed in the latest 3.0 alpha. Once the next stable version is released, you can use VS2013.

###Installing Dependencies

1. Install Visual Studio 2012 (VS2012 codenamed vc11). Install latest update which is "Update 4" at this time. Make sure to install VS2012 <b>before</b> CUDA drivers so the CUDA installer can install the VS Plugin properly.
2. [Download CUDA 6.5](https://developer.nvidia.com/cuda-downloads). Select custom install. Check everything except 3D Vision as I don't use it.  Let the installer override your current graphics drivers if yours is the older version.
3. [Download OpenCV 2.4.9](https://github.com/Itseez/opencv/releases) source zip. Do not use the installable exe as that does not come with CUDA support. Unzip to `C:\opencv`
4. [Download Cmake 3.0.1 Win32 installer](http://www.cmake.org/cmake/resources/software.html). During installation, add Cmake to system path of all users.

###Compiling OpenCV
0. As a shortcut, if you intend to use the the exact same dependencies namely OpenCV 2.4.9, VS2012 and a Kepler GPU with CC3.0, you can just use my compilation. Unzip this [file](https://github.com/yeokm1/ssim-cuda/releases/download/v0.0/opencv249cuda.zip) to `C:\` and skip the rest of these steps.
1. Open CMake GUI as administrator so it access all directories.
2. Set source to `C:\opencv` and where-to-build to `C:\opencv\build`
3. Click "Configure", choose Visual Studio 11 2012, "Default native compilers". We are using the 32-bit compiler.
4. Search for CUDA. For `CUDA_ARCH_BIN`, remove all the numbers except the number of your current GPU architecture. For example, I use a GeForce 650 which has a compute capability (CC) of 3.0. Use this [list](https://developer.nvidia.com/cuda-gpus) to look up the CC of your GPU. We remove other numbers to shorten compilation time by only compiling for the CC we want.
5. Empty out the `CUDA_ARCH_PTX` field as I don't use a virtual platform. Tick `CUDA_FAST_MATH`. Verify that `WITH_CUDA` and `BUILD_opencv_gpu` are also ticked. Check that `CUDA_TOOLKIT_ROOT_DIR` is correctly populated.
6. To speed up compilation even more, I untick `BUILD_TESTS` and `BUILD_PERF_TESTS`.
7. Click Generate.
8. Go to `C:\opencv\build` and open OPENCV.sln. Select Debug Build.
9. Click BUILD in the toolbar and BUILD ALL_BUILD. If VS asks to reload modules because they have been modified externally, just reload.
10. Select Release Build and repeat step 8.
11. Go to System Properties to set your PATH. Modify PATH by adding, `c:\opencv\build\bin\DEBUG;c:\opencv\build\bin\RELEASE`

##Project Settings (for new projects)
This is optional as I have already set the settings in the current project. If you want to use your compiled OpenCV installation for other new projects, these are steps you should configure.

1. Set your project to Debug mode.
2. Right click on your project name on the sidebar like "ssim-cuda" and select Properties. 
3. Go to: Configuration Properties -> C/C++ -> General -> Additional Include Directories. Add the include paths of the modules you are using. For ssim-cuda, that would be
 * C:\opencv\modules\core\include
 * C:\opencv\modules\imgproc\include
 * C:\opencv\modules\highgui\include
 * C:\opencv\modules\gpu\include
 * C:\opencv\modules\objdetect\include
 * C:\opencv\modules\features2d\include
 * C:\opencv\modules\flann\include
4. Go to: Configuration Properties -> Linker -> General -> Additional Library Directories. Set to `C:\opencv\build\lib\Debug`. 
5. Go to: Configuration Properties -> Linker -> Input -> Additional Dependencies. Add these
 * opencv_calib3d249d.lib
 * opencv_contrib249d.lib
 * opencv_core249d.lib
 * opencv_features2d249d.lib
 * opencv_flann249d.lib
 * opencv_gpu249d.lib
 * opencv_highgui249d.lib
 * opencv_imgproc249d.lib
 * opencv_legacy249d.lib
 * opencv_ml249d.lib
 * opencv_nonfree249d.lib
 * opencv_objdetect249d.lib
 * opencv_ocl249d.lib
 * opencv_photo249d.lib
 * opencv_stitching249d.lib
 * opencv_superres249d.lib
 * opencv_ts249d.lib
 * opencv_video249d.lib
 * opencv_videostab249d.lib
6. Set your project to Release mode.
7. Repeat Step 2 and 3.
8. Go to: Configuration Properties -> Linker -> General -> Additional Library Directories. Set to `C:\opencv\build\lib\Release`.
9. Go to: Configuration Properties -> Linker -> Input -> Additional Dependencies. Add these
 * opencv_calib3d249.lib
 * opencv_contrib249.lib
 * opencv_core249.lib
 * opencv_features2d249.lib
 * opencv_flann249.lib
 * opencv_gpu249.lib
 * opencv_highgui249.lib
 * opencv_imgproc249.lib
 * opencv_legacy249.lib
 * opencv_ml249.lib
 * opencv_nonfree249.lib
 * opencv_objdetect249.lib
 * opencv_ocl249.lib
 * opencv_photo249.lib
 * opencv_stitching249.lib
 * opencv_superres249.lib
 * opencv_ts249.lib
 * opencv_video249.lib
 * opencv_videostab249.lib

##References
 * [Similarity check (PNSR and SSIM) on the GPU](http://docs.opencv.org/doc/tutorials/gpu/gpu-basics-similarity/gpu-basics-similarity.html)
 * [Configuring CMake options](http://answers.opencv.org/question/13490/cmake-opencv245-git-repository-24-branch-windows-7/)
 * [CMake CUDA options](http://www.programmerfish.com/how-to-build-opencv-2-4-6-with-gpu-module-in-windows/#.U_xsUPmSwjW)
 * [CMake CUDA optimisations](http://answers.opencv.org/question/5090/why-opencv-building-is-so-slow-with-cuda/)
 * [Configuring system PATH and VS project options](http://opencv-srf.blogspot.sg/2013/05/installing-configuring-opencv-with-vs.html)
