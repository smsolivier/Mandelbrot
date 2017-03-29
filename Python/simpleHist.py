#!/usr/bin/python3

import numpy as np 
import matplotlib.pyplot as plt 

hist = np.loadtxt('Data/hist')

kVal = np.loadtxt('Data/iter')

cf = np.zeros(len(hist))

cf[0] = hist[0]

for i in range(1, len(hist)):
	cf[i] = cf[i-1] + hist[i]

max = np.max(cf)

cf /= max

image = np.zeros((np.shape(kVal)[0], np.shape(kVal)[1]))

for i in range(np.shape(kVal)[0]):
	for j in range(np.shape(kVal)[1]):
		k = int(kVal[i][j])
		image[i][j] = cf[k-1]

plt.imshow(image)
plt.show()