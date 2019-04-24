#!/usr/bin/env python 

import glob
import os 
import subprocess

def main():

	grdfiles = glob.glob('*.grd')

	for infile in grdfiles:

		print 'Dealing with %s' %infile

		p = subprocess.Popen(['gmt', 'grdinfo', '%s' %infile],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
		out,err = p.communicate()

		#grab thhe min and max values so we can use them to generate a cpt file
		mmline = out.split('\n')[7].split(' ')
		minval = '%.2f' %float(mmline[2])
		maxval = '%.2f' %float(mmline[4])

		print minval,maxval

		if minval == maxval:
			print 'Error: no variation in phase velocity seen in file %s' %infile
		else:
			os.system('./map_phase_vel.sh %s %s %s' %(infile,minval,maxval))


if __name__ == '__main__':

	main()
