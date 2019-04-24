#!/usr/bin/env python

#RMS March 2017
#Adust the standard colorscheme file such that it spans an input range

import numpy as np
import sys 


def main():

	vmin=float(sys.argv[1])
	vmax=float(sys.argv[2])

	vvals = np.linspace(vmin,vmax,12)

	COL1 = vvals[:-1]
	COL2 = [166,255,255,255,255,255,255,152,90,26,0,0]
	COL3 = [0,0,75,132,198,250,255,250,205,160,100,50]
	COL4 = [0,0,0,0,0,152,255,255,255,255,255,200]
	COL5 = vvals[1:]
	COL6 = [225,255,255,255,255,255,152,90,26,0,0,0]
	COL7 = [0,75,132,198,250,255,250,205,160,100,50,10]
	COL8 = [0,0,0,0,152,255,255,255,255,255,200,160]

	outfile = open('vels.cpt','w')

	for element in zip(COL1,COL2,COL3,COL4,COL5,COL6,COL7,COL8):

		outfile.write('\t%.4g\t%g\t%g\t%g\t%.4g\t%g\t%g\t%g\n' %(element[0],element[1],element[2],element[3],element[4],element[5],element[6],element[7]))

	outfile.write('B\t166\t0\t0\n')
	outfile.write('F\t0\t10\t160\n')
	outfile.write('N\t255\t255\t255')


if __name__ == '__main__':

	main()

