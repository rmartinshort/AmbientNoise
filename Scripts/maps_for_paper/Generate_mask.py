#!/usr/bin/env python 
#Generate a mask for the ambient noise and teleseismic surface wave tomo maps
#that is consistent with the area displayed in the final models
#For ambient noise paper figure 3
#RMS Jan 2018

import numpy as np

def main():

	latmin = 58.2
	latmax = 68.85
	lonmin = 201.2
	lonmax = 224.25

	lons = np.arange(lonmin,lonmax,0.1)
	lats = np.arange(latmin,latmax,0.1)

	#Write the coordinates of the points within the region we want to display 
	maskfile = open('mask_coords.dat','w')

	for lon in lons:
		for lat in lats:

			y = mask_line(lon)

			if (lat <= y):
				if (lat <= 70.0):
					maskfile.write('%g %g\n' %(lon,lat))

	maskfile.close()


def mask_line(x):

	'''Equation of line dividing the masked region from the non-masked region'''
	x = x - 360
	c = -4.2
	m = -0.5
	y = m*x + c 
	return y





if __name__ == '__main__':

	main()