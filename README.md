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
1. OpenCV (2.4.9)
2. Cmake (3.0.1)
3. Visual Studio 2012 Update 4
4. Nvidia Cuda



