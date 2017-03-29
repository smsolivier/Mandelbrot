#!/usr/bin/env python

import numpy as np 
import matplotlib.pyplot as plt 
from matplotlib.colors import LogNorm

import os

import time 

def mandelbrot(c, maxIter):
	''' Calculates whether a complex numer c is in the Mandelbrot Set '''
	z = c
	k = 0 # give k the correct scope 
	for k in range(maxIter):
		if (abs(z) > 2): # not in set if |z| > 2
			return k # breaks the loop and returns k

		z = z**2 + c

	return k # in set if makes it through the loop 

# def mandelbrot(c, maxIter):
# 	''' Calculates whether a complex numer c is in the Mandelbrot Set '''
# 	z = c
# 	for k in range(maxIter):
# 		if (abs(z) > 2): # not in set if |z| > 2
# 			return 0 # breaks the loop and returns 0

# 		z = z**2 + c

# 	return 1 # in set if makes it through the loop 

# def mandelbrotSet(x, y, radius, maxIter, resolution):
# 	''' Prints the Mandelbrot Set
# 		Inputs:
# 			x:  real value of the center of the image
# 			y:  imaginary value of the center
# 			radius:  distance from center to edge of image 
# 			maxIter:  number of loops before exiting
# 			resolution:  width and height of image in pixels 
# 	''' 
# 	minReal = x - radius # min value on the x axis 
# 	maxReal = x + radius # max value on the x axis 

# 	minImag = y - radius # min value on the y axis
# 	maxImag = y + radius # max value on the y axis 

# 	# arrays of points equally spaced between min and max 
# 	real = np.linspace(minReal, maxReal, resolution)
# 	imag = np.linspace(minImag, maxImag, resolution)

# 	# let 0 = in set and 1 = out of set (for plotting purposes)
# 	image = np.zeros((resolution, resolution))

# 	for i in range(resolution): # loop through imaginary values
# 		for j in range(resolution): # loop through real values 
# 			c = complex(real[j], imag[i]) # set value of c

# 			if (mandelbrot(c, maxIter) == 0): # if not in set 
# 				image[i][j] = 1

# 		print(float(i)/resolution, end='\r') # progress bar 

# 	return image

def mandelbrotSet(x, y, radius, maxIter, resolution):
	''' Prints the Mandelbrot Set
		Inputs:
			x:  real value of the center of the image
			y:  imaginary value of the center
			radius:  distance from center to edge of image 
			maxIter:  number of loops before exiting
			resolution:  width and height of image in pixels 
	''' 
	minReal = x - radius # min value on the x axis 
	maxReal = x + radius # max value on the x axis 

	minImag = y - radius # min value on the y axis
	maxImag = y + radius # max value on the y axis 

	# arrays of points equally spaced between min and max 
	real = np.linspace(minReal, maxReal, resolution)
	imag = np.linspace(minImag, maxImag, resolution)

	# let 0 = in set and 1 = out of set (for plotting purposes)
	image = np.zeros((resolution, resolution))

	for i in range(resolution): # loop through imaginary values
		for j in range(resolution): # loop through real values 
			c = complex(real[j], imag[i]) # set value of c

			image[i][j] = mandelbrot(c, maxIter)

	return image

def zoom(x, y, r_init, r_final, maxIter, resolution, numZooms):
	dzoom = np.exp(1/numZooms * np.log(r_init/r_final))

	for i in range(numZooms):
		r = r_init/dzoom**i
		image = mandelbrotSet(x, y, r, maxIter, resolution)

		plt.figure(figsize=(16, 12))
		plt.imshow(image, norm=LogNorm()) # plot the array 
		plt.savefig('Movie/image'+str(i)+'.png')
		plt.close()

		print(float(i)/numZooms, end='\r') # progress bar 

	os.system('ffmpeg -f image2 -r 15 -i Movie/image%d.png -b 320000k out.mp4')

start = time.time()
zoom(-0.235131, 0.827206, 1.7, 8e-8, 1000, 1000, 25)
end = time.time()

print('Wall Time =', end - start)