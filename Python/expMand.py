#!/usr/bin/python3

import matplotlib.pyplot as plt 
import numpy as np 
from matplotlib.colors import LogNorm

def isInSet(x, y, maxIterations=1000):
	a = 0 
	b = 0
	k = 0
	for k in range(maxIterations):
		temp = a**2 - b**2 + x; 
		b = 2 * a * b + y
		a = temp

		if (a**2 + b**2 > 2**2):
			return k

	return k

def printImage(x, y, radius, resolution):
	minReal = x - radius # minimum value on the x axis 
	maxReal = x + radius # max value on the x axis 

	minImaginary = y - radius # min imaginary value
	maxImaginary = y + radius # max imaginary value 

	x = np.linspace(minReal, maxReal, resolution)
	y = np.linspace(minImaginary, maxImaginary, resolution)

	# stores image
	image = np.zeros((resolution, resolution)) 
	c_re = 0 # real part of c
	c_im = 0 # imaginary part of c

	for i in range(resolution):
		c_im = y[i]
		for j in range(resolution):
			c_re = x[i]
			image[i][j] = isInSet(c_re, c_im)

		print(float(i)/resolution, end='\r') # simple progress bar

	plt.figure(figsize=(16, 12))
	plt.imshow(image, norm=LogNorm(), cmap='hot')
	# plt.gray()
	plt.show()

printImage(x=-.5, y=0, radius=1.35, resolution=100)