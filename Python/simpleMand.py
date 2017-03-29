#!/usr/bin/python3

import matplotlib.pyplot as plt 
import numpy as np 

def isInSet(x, y, maxIterations=1000):
	a = 0 
	b = 0
	inSet = 1
	for k in range(maxIterations):
		temp = a**2 - b**2 + x; 
		b = 2 * a * b + y
		a = temp

		if (a**2 + b**2 > 2**2):
			inSet = 0
			break

	return inSet

def printImage(x, y, radius, resolution):
	minReal = x - radius # minimum value on the x axis 
	maxReal = x + radius # max value on the x axis 

	minImaginary = y - radius # min imaginary value
	maxImaginary = y + radius # max imaginary value 

	x = np.linspace(minReal, maxReal, resolution)
	y = np.linspace(minImaginary, maxImaginary, resolution)

	# let 0 = in set and 1 = out of set (for plotting purposes)
	image = np.zeros((resolution, resolution)) # store isInSet values. Default is in set
	c_re = 0 # real part of c
	c_im = 0 # imaginary part of c

	for i in range(resolution):
		c_im = y[i]
		for j in range(resolution):
			c_re = x[j]

			if (isInSet(c_re, c_im) == 0): # if not in set 
				image[i][j] = 1
		print(i/resolution)

	plt.figure(figsize=(8, 8))
	plt.imshow(image, extent=[minReal, maxReal, minImaginary, maxImaginary])
	plt.gray()
	plt.savefig('pymand.png')

printImage(x=-.5, y=0, radius=1.35, resolution=1000)
