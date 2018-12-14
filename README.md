# image-processing-hw2

## 1) HW_errDiffusion
Reads the input image from I1 and converts it into a pseudo gray-scale image using the error diffusion algorithm. 

## 2) Function HW_blur 
Reads input image from I1 and blurs it with a box filter (unweighted averaging) using a separable implementation.

## 3) Function HW_sharpen
sharpens the image in I1 by subtracting a blurred version of I1 from its original values and adding the scaled difference back to I1

## 4) Function HW_median
applies a median filter to I1 overaneighborhood size of sz×sz.

## 5) Function HW_convolve
convolves the input image I1 with kernel and store the result in I2.

