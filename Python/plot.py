#!/usr/bin/python3
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
import numpy as np

a = np.loadtxt('Data/image')

for i in range(np.shape(a)[0]):
    for j in range(np.shape(a)[1]):
        if (a[i][j] == 1):
            a[i][j] = 0
        elif (a[i][j] == 0):
            a[i][j] = 1

plt.imshow(a, cmap='viridis')
plt.gray()
plt.show()
